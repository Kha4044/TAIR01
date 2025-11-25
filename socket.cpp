#include "socket.h"
#include <QDebug>

#define DEFAULT_TIMEOUT 15000
#define DEFAULT_OPC_TIMEOUT 45000
#define DEFAULT_FDAT_INTERVAL 2000

Socket::Socket(QObject* parent)
    : VNAclient(parent)
    , _socket(nullptr)
    , _fdatTimer(nullptr)
    , _scanning(false)
    , _powerMeasuring(false)
    , _lastType(1)
    , _currentGraphCount(1)
    , _normalTimeout(DEFAULT_TIMEOUT)
    , _opcTimeout(DEFAULT_OPC_TIMEOUT)
    , _fdatInterval(DEFAULT_FDAT_INTERVAL)
    , _powerStartKHz(20)
    , _powerStopKHz(4800000)
    , _powerPoints(201)
    , _powerBand(10000)
    , _thread(nullptr)
{
    _thread = new QThread();
    this->moveToThread(_thread);
    connect(_thread, &QThread::started, this, &Socket::initializeInThread);
    connect(_thread, &QThread::finished, this, &Socket::cleanupInThread);
}

Socket::~Socket()
{
    stopThread();
}

VNAclient* Socket::getInstance()
{
    return this;
}

void Socket::setTimeouts(int normalTimeout, int opcTimeout, int fdatInterval)
{
    _normalTimeout = normalTimeout;
    _opcTimeout = opcTimeout;
    _fdatInterval = fdatInterval;

    if (_fdatTimer && _fdatTimer->isActive()) {
        _fdatTimer->setInterval(_fdatInterval);
    }
}

void Socket::startThread()
{
    if (_thread && !_thread->isRunning())
        _thread->start();
}

void Socket::stopThread()
{
    if (_thread && _thread->isRunning()) {
        _scanning = false;
        _powerMeasuring = false;

        if (_fdatTimer) {
            _fdatTimer->stop();
        }

        QMetaObject::invokeMethod(this, "cleanupInThread", Qt::QueuedConnection);
        _thread->quit();

        if (!_thread->wait(3000)) {
            _thread->terminate();
            _thread->wait();
        }
    }
}

void Socket::initializeInThread()
{
    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::connected, this, &Socket::onConnected);
    connect(_socket, &QTcpSocket::disconnected, this, &Socket::onDisconnected);
    connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError socketError) {
                if (!_scanning && !_powerMeasuring) return;

                qDebug() << "❌ Ошибка сокета:" << socketError << _socket->errorString();
                if (socketError != QAbstractSocket::RemoteHostClosedError) {
                    emit error(socketError, _socket->errorString());
                }
            });

    _fdatTimer = new QTimer(this);
    _fdatTimer->setInterval(_fdatInterval);
    _fdatTimer->setSingleShot(false);
    connect(_fdatTimer, &QTimer::timeout, this, &Socket::requestFDAT);

    qDebug() << "✅ Сокет инициализирован в потоке:" << QThread::currentThread();
    qDebug() << "⚙️ Настройки таймаутов - Normal:" << _normalTimeout << "ms, OPC:" << _opcTimeout << "ms, FDAT:" << _fdatInterval << "ms";
}

void Socket::cleanupInThread()
{
    qDebug() << "🧹 Очистка сокета...";

    if (_fdatTimer) {
        _fdatTimer->stop();
    }

    if (_socket) {
        if (_socket->state() == QAbstractSocket::ConnectedState) {
            _socket->disconnectFromHost();
            if (!_socket->waitForDisconnected(1000)) {
                _socket->abort();
            }
        }
    }

    qDebug() << "✅ Очистка завершена";
}

void Socket::setGraphSettings(int graphCount, const QVector<int>& traceNumbers)
{
    _currentGraphCount = graphCount;
    _activeTraceNumbers = traceNumbers;
}

bool Socket::ensureConnection(const QHostAddress& host, quint16 port)
{
    if (!_socket) {
        qDebug() << "❌ Сокет не инициализирован";
        return false;
    }

    if (_socket->state() == QAbstractSocket::ConnectedState &&
        _host == host && _port == port) {
        return true;
    }

    if (_socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "🔁 Переподключение к новому хосту...";
        _socket->disconnectFromHost();
        if (!_socket->waitForDisconnected(1000)) {
            _socket->abort();
        }
    }

    qDebug() << "🔗 Подключение к" << host.toString() << ":" << port;
    _socket->connectToHost(host, port);

    if (!_socket->waitForConnected(_normalTimeout)) {
        QAbstractSocket::SocketError socketError = _socket->error();
        QString errorString = _socket->errorString();

        qDebug() << "❌ Не удалось подключиться за" << _normalTimeout << "ms";
        qDebug() << "Код ошибки:" << socketError;
        qDebug() << "Текст ошибки:" << errorString;

        emit error(socketError, errorString);
        return false;
    }

    _host = host;
    _port = port;
    QThread::msleep(100);

    qDebug() << "✅ Подключение установлено";
    return true;
}

bool Socket::waitForOperationsComplete(int timeoutMs)
{
    if (!_socket || _socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "❌ Нет подключения для OPC";
        return false;
    }

    qDebug() << "⏳ Ожидание завершения операций (" << timeoutMs << "ms)...";

    _socket->readAll();
    _socket->write("*OPC?\n");

    if (!_socket->flush()) {
        qDebug() << "❌ Ошибка отправки OPC команды";
        return false;
    }

    if (!_socket->waitForReadyRead(timeoutMs)) {
        qDebug() << "❌ Таймаут ожидания OPC (" << timeoutMs << "ms)";
        return false;
    }

    QByteArray response = _socket->readAll().trimmed();
    bool ok = (response == "1");

    if (ok) {
        qDebug() << "✅ Все операции завершены";
    } else {
        qDebug() << "❌ OPC вернул неожиданный ответ:" << response;
    }

    return ok;
}

void Socket::sendCommandWithOPC(const QHostAddress& host, quint16 port, const QVector<VNAcomand*>& commands)
{
    if (!ensureConnection(host, port)) {
        qDeleteAll(commands);
        return;
    }

    for (auto *cmd : commands) {
        QByteArray ba = cmd->SCPI.toUtf8();
        qDebug() << "📤 Отправка:" << ba.trimmed();
        _socket->write(ba);
        _socket->flush();

        if (!cmd->request) {
            delete cmd;
            continue;
        }

        if (_socket->waitForReadyRead(_normalTimeout)) {
            QByteArray resp = _socket->readAll();
            emit dataFromVNA(QString::fromUtf8(resp), cmd);
        } else {
            qDebug() << "❌ Таймаут ожидания ответа на команду:" << ba.trimmed();
            delete cmd;
        }
    }

    if (!waitForOperationsComplete(_opcTimeout)) {
        qDebug() << "⚠️ OPC не завершился, но продолжаем работу";
    }
}

void Socket::sendCommand(const QHostAddress& host, quint16 port, const QVector<VNAcomand*>& commands)
{
    if (QThread::currentThread() != _thread) {
        auto *copy = new QVector<VNAcomand*>();
        for (auto *cmd : commands)
            copy->append(new VNAcomand(cmd->request, cmd->type, cmd->SCPI));

        QMetaObject::invokeMethod(
            this, "sendCommand", Qt::QueuedConnection,
            Q_ARG(QHostAddress, host),
            Q_ARG(quint16, port),
            Q_ARG(QVector<VNAcomand*>, *copy)
            );

        qDeleteAll(*copy);
        delete copy;
        return;
    }

    sendCommandImpl(host, port, commands);
}

void Socket::sendCommandImpl(const QHostAddress& host, quint16 port, const QVector<VNAcomand*>& commands)
{
    if (!ensureConnection(host, port)) {
        qDeleteAll(commands);
        return;
    }

    for (auto *cmd : commands) {
        QByteArray ba = cmd->SCPI.toUtf8();
        qDebug() << "📤 Отправка команды:" << ba.trimmed();
        _socket->write(ba);

        if (!_socket->flush()) {
            qDebug() << "❌ Ошибка отправки данных";
            delete cmd;
            continue;
        }

        if (!cmd->request) {
            delete cmd;
            continue;
        }

        int timeout = _normalTimeout;
        if (cmd->SCPI.contains("FDAT") || cmd->SCPI.contains("XAXIS")) {
            timeout = 30000;
        }

        if (_socket->waitForReadyRead(timeout)) {
            QByteArray resp = _socket->readAll();
            qDebug() << "📨 Получено" << resp.size() << "байт в ответ на:" << ba.trimmed();
            emit dataFromVNA(QString::fromUtf8(resp), cmd);
        } else {
            qDebug() << "❌ Таймаут ожидания ответа (" << timeout << "ms) на команду:" << ba.trimmed();
        }
    }
}

void Socket::startScan(const QString& ip, quint16 port, int startKHz, int stopKHz, int points, int band)
{
    if (QThread::currentThread() != _thread) {
        QMetaObject::invokeMethod(
            this, "startScan", Qt::QueuedConnection,
            Q_ARG(QString, ip),
            Q_ARG(quint16, port),
            Q_ARG(int, startKHz),
            Q_ARG(int, stopKHz),
            Q_ARG(int, points),
            Q_ARG(int, band)
            );
        return;
    }

    qDebug() << "🟢 START SCAN В ПОТОКЕ СОКЕТА";
    qDebug() << "IP:" << ip << "Port:" << port;
    qDebug() << "Freq:" << startKHz << "-" << stopKHz << "kHz, Points:" << points;

    QHostAddress addr;
    if (!addr.setAddress(ip)) {
        qDebug() << "❌ ОШИБКА: Неверный IP адрес";
        emit error(-1, QString("Неверный IP: %1").arg(ip));
        return;
    }

    qint64 startHz = qint64(startKHz) * 1000LL;
    qint64 stopHz = qint64(stopKHz) * 1000LL;
    qint64 bwHz = qint64(band);

    qDebug() << "⚙️ Настройка параметров VNA...";

    QVector<VNAcomand*> cmds;
    cmds.append(new SYSTEM_PRESET());
    cmds.append(new SENS_FREQ_START(1, startHz));
    cmds.append(new SENS_FREQ_STOP(1, stopHz));
    cmds.append(new SENS_SWE_POINT(1, points));
    cmds.append(new SENS_BWID(1, bwHz));
    cmds.append(new TRIGGER_SOURCE_BUS());
    cmds.append(new INITIATE_CONTINUOUS(1));

    sendCommandWithOPC(addr, port, cmds);

    if (!_scanning) {
        _scanning = true;
        if (_fdatTimer) {
            _fdatTimer->start();
            qDebug() << "Таймер FDAT запущен с интервалом" << _fdatInterval << "ms";
        }
    }

    qDebug() << "✅ СКАНИРОВАНИЕ НАСТРОЕНО";
}

void Socket::stopScan()
{
    if (QThread::currentThread() != _thread) {
        QMetaObject::invokeMethod(this, "stopScan", Qt::QueuedConnection);
        return;
    }

    if (!_scanning) return;

    qDebug() << "🛑 Остановка сканирования...";
    _scanning = false;

    if (_fdatTimer) {
        _fdatTimer->stop();
        qDebug() << "⏹️ Таймер FDAT остановлен";
    }

    QVector<VNAcomand*> cmds;
    cmds.append(new ABORT_COMMAND());
    cmds.append(new INITIATE_SINGLE_SHOT(1));

    int savedTimeout = _normalTimeout;
    _normalTimeout = 5000;
    sendCommandImpl(_host, _port, cmds);
    _normalTimeout = savedTimeout;

    qDebug() << "✅ Сканирование остановлено";
}

void Socket::requestFDAT()
{
    if (!_scanning && !_powerMeasuring) return;
    if (_activeTraceNumbers.isEmpty()) return;
    if (!_socket || _socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "❌ Нет активного подключения для запроса FDAT";
        return;
    }

    static bool isProcessing = false;
    if (isProcessing) {
        qDebug() << "⏸️ Пропускаем FDAT - предыдущий запрос еще выполняется";
        return;
    }
    isProcessing = true;

    qDebug() << "📊 requestFDAT: начало получения данных...";

    // 1) Отправляем программный триггер для BUS-источника: TRIG:SING (если TRIG:SOUR BUS установлен)
    qDebug() << "🎯 Отправка BUS триггера (TRIG:SING)...";
    QVector<VNAcomand*> trigCmds;
    trigCmds.append(new TRIGGER_SINGLE());
    sendCommandImpl(_host, _port, trigCmds); // отправка без удаления ответа (команда не request)

    // 2) Ждём завершения измерения через OPC
    qDebug() << "⏳ Ожидание завершения сканирования (OPC)...";
    if (!waitForOperationsComplete(_opcTimeout)) {
        qDebug() << "⚠️ Таймаут ожидания OPC - продолжим попытку чтения данных";
        // мы не возвращаемся — пытаемся прочитать данные в любом случае
    }

    // 3) Запрашиваем X-axis
    qDebug() << "📈 Получение данных X-оси...";
    QVector<VNAcomand*> fx;
    fx.append(new CALC_TRACE_DATA_XAXIS(_activeTraceNumbers.first()));
    sendCommandImpl(_host, _port, fx); // sendCommandImpl отправит и вызовет emit dataFromVNA

    // 4) Запрашиваем данные трасс
    for (int tr : _activeTraceNumbers) {
        qDebug() << "📊 Получение данных трассы" << tr << "...";
        QVector<VNAcomand*> cmds;
        if (_powerMeasuring)
            cmds.append(new CALC_TRACE_DATA_POWER(tr));
        else
            cmds.append(new CALC_TRACE_DATA_FDAT(tr));
        sendCommandImpl(_host, _port, cmds);
    }

    qDebug() << "🎉 requestFDAT: чтение данных инициировано";
    // небольшая пауза чтобы UI успел обработать входящие сигналы
    QThread::msleep(20);

    isProcessing = false;
}

void Socket::startPowerMeasurement(int startKHz, int stopKHz, int points, int band)
{
    _powerStartKHz = startKHz;
    _powerStopKHz = stopKHz;
    _powerPoints = points;
    _powerBand = band;

    QVector<VNAcomand*> cmds;
    cmds.append(new SYSTEM_PRESET());
    cmds.append(new SOURCE_POWER_COUPLE(1, false));
    cmds.append(new SOURCE_POWER_LEVEL_SET(1, 0.0));
    cmds.append(new OUTPUT_PORT_STATE(1, true));
    cmds.append(new CALC_PARAMETER_POWER(1, "R1"));
    cmds.append(new CALC_TRACE_FORMAT_POWER(1, "MLOG"));
    cmds.append(new TRIGGER_SOURCE_BUS());
    cmds.append(new INITIATE_CONTINUOUS(1));

    sendCommandWithOPC(_host, _port, cmds);

    if (!_powerMeasuring) {
        _powerMeasuring = true;
        if (_fdatTimer) {
            _fdatTimer->start();
            qDebug() << "📊 Измерение мощности запущено";
        }
    }
}

void Socket::stopPowerMeasurement()
{
    if (!_powerMeasuring) return;

    qDebug() << "🛑 Остановка измерения мощности...";
    _powerMeasuring = false;

    if (_fdatTimer) {
        _fdatTimer->stop();
    }

    QVector<VNAcomand*> cmds;
    cmds.append(new ABORT_COMMAND());
    cmds.append(new INIT_CONT_MODE(1, false));

    int savedTimeout = _normalTimeout;
    _normalTimeout = 5000;
    sendCommandImpl(_host, _port, cmds);
    _normalTimeout = savedTimeout;

    qDebug() << "✅ Измерение мощности остановлено";
}

void Socket::onConnected()
{
    qDebug() << "🔗 Подключение установлено к" << _host.toString() << ":" << _port;
    emit connected();
}

void Socket::onDisconnected()
{
    qDebug() << "🔌 Подключение разорвано";

    _scanning = false;
    _powerMeasuring = false;

    if (_fdatTimer) {
        _fdatTimer->stop();
    }

    emit disconnected();
}

bool Socket::canConnect(const QString &ip, quint16 port)
{
    qDebug() << "=== ДИАГНОСТИКА ПОДКЛЮЧЕНИЯ ===";
    qDebug() << "Цель:" << ip << ":" << port;

    QTcpSocket testSocket;
    testSocket.connectToHost(ip, port);

    bool connected = testSocket.waitForConnected(5000);

    qDebug() << "Результат подключения:" << connected;

    if (!connected) {
        qDebug() << "Код ошибки:" << testSocket.error();
        qDebug() << "Текст ошибки:" << testSocket.errorString();
    }

    if (connected) {
        testSocket.disconnectFromHost();
        testSocket.waitForDisconnected(1000);
    }

    qDebug() << "=== КОНЕЦ ДИАГНОСТИКИ ===";
    return connected;
}
