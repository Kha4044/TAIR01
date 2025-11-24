#include "socket.h"
#include "vnaclient.h"
#include <QDebug>
#include <QThread>

#define TIMEOUT 5000
#define FDAT_INTERVAL 500
#define OPC_TIMEOUT 30000

Socket::Socket(QObject* parent)
    : VNAclient(parent)
    , _socket(nullptr)
    , _fdatTimer(nullptr)
    , _scanning(false)
    , _powerMeasuring(false)
    , _lastType(1)
    , _currentGraphCount(1)
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

void Socket::startThread()
{
    if (_thread && !_thread->isRunning())
        _thread->start();
}

void Socket::stopThread()
{
    if (_thread && _thread->isRunning()) {
        QMetaObject::invokeMethod(this, "stopScan", Qt::BlockingQueuedConnection);
        QMetaObject::invokeMethod(this, "stopPowerMeasurement", Qt::BlockingQueuedConnection);
        _thread->quit();
        _thread->wait();
    }
}

void Socket::initializeInThread()
{
    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::connected, this, &Socket::onConnected);
    connect(_socket, &QTcpSocket::disconnected, this, &Socket::onDisconnected);
    connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError socketError) {
                qDebug() << "❌ Ошибка сокета:" << socketError << _socket->errorString();
                emit error(socketError, _socket->errorString());
            });

    _fdatTimer = new QTimer(this);
    _fdatTimer->setInterval(FDAT_INTERVAL);
    _fdatTimer->setSingleShot(false);
    connect(_fdatTimer, &QTimer::timeout, this, &Socket::requestFDAT);

    qDebug() << "✅ Сокет инициализирован в потоке:" << QThread::currentThread();
}

void Socket::cleanupInThread()
{
    if (_fdatTimer)
        _fdatTimer->stop();

    if (_socket && _socket->state() == QAbstractSocket::ConnectedState) {
        _socket->disconnectFromHost();
        _socket->waitForDisconnected(500);
    }
}

void Socket::setGraphSettings(int graphCount, const QVector<int>& traceNumbers)
{
    _currentGraphCount = graphCount;
    _activeTraceNumbers = traceNumbers;
}

bool Socket::ensureConnection(const QHostAddress& host, quint16 port)
{
    if (!_socket) return false;

    if (_socket->state() == QAbstractSocket::ConnectedState && _host == host && _port == port) {
        return true;
    }

    // Если подключены к другому хосту/порту - переподключаемся
    if (_socket->state() == QAbstractSocket::ConnectedState) {
        _socket->disconnectFromHost();
        _socket->waitForDisconnected(1000);
    }

    qDebug() << "🔗 Подключение к" << host.toString() << ":" << port;
    _socket->connectToHost(host, port);
    if (!_socket->waitForConnected(TIMEOUT)) {
        emit error(_socket->error(), _socket->errorString());
        return false;
    }

    _host = host;
    _port = port;
    return true;
}

bool Socket::waitForOperationsComplete(int timeoutMs)
{
    if (!_socket || _socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "❌ Нет подключения для OPC";
        return false;
    }

    qDebug() << "⏳ Ожидание завершения операций...";
    _socket->write("*OPC?\n");
    _socket->flush();

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

    // Отправляем все команды
    for (auto *cmd : commands) {
        QByteArray ba = cmd->SCPI.toUtf8();
        qDebug() << "📤 Отправка:" << ba.trimmed();
        _socket->write(ba);
        _socket->flush();

        if (!cmd->request) {
            delete cmd;
            continue;
        }

        if (_socket->waitForReadyRead(TIMEOUT)) {
            QByteArray resp = _socket->readAll();
            emit dataFromVNA(QString::fromUtf8(resp), cmd);
        } else {
            qDebug() << "❌ Таймаут ожидания ответа на команду:" << ba.trimmed();
            delete cmd;
        }
    }

    // ЖДЕМ ЗАВЕРШЕНИЯ ВСЕХ ОПЕРАЦИЙ
    waitForOperationsComplete(OPC_TIMEOUT);
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
        _socket->write(ba);
        _socket->flush();

        if (!cmd->request) {
            delete cmd;
            continue;
        }

        if (_socket->waitForReadyRead(TIMEOUT)) {
            QByteArray resp = _socket->readAll();
            emit dataFromVNA(QString::fromUtf8(resp), cmd);
        } else {
            qDebug() << "❌ Таймаут ожидания ответа";
            delete cmd;
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

    // Настройка параметров сканирования
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
    cmds.append(new INIT_CONT_MODE(1, true));
    cmds.append(new TRIGGER_SOURCE("IMM"));

    // ОТПРАВЛЯЕМ КОМАНДЫ С ОЖИДАНИЕМ ЗАВЕРШЕНИЯ
    sendCommandWithOPC(addr, port, cmds);

    if (!_scanning) {
        _scanning = true;
        if (_fdatTimer) {
            _fdatTimer->start();
            qDebug() << "Таймер FDAT запущен";
        }
    }

    qDebug() << "✅ СКАНИРОВАНИЕ ЗАПУЩЕНО И НАСТРОЙКИ ПРИМЕНЕНЫ";
}

void Socket::stopScan()
{
    if (QThread::currentThread() != _thread) {
        QMetaObject::invokeMethod(this, "stopScan", Qt::QueuedConnection);
        return;
    }

    if (!_scanning) return;

    _scanning = false;
    if (_fdatTimer) _fdatTimer->stop();

    QVector<VNAcomand*> cmds;
    cmds.append(new ABORT_COMMAND());
    cmds.append(new INIT_CONT_MODE(1, false));
    sendCommandImpl(_host, _port, cmds);

    qDebug() << "🛑 Сканирование остановлено";
}

void Socket::requestFDAT()
{
    if (!_scanning && !_powerMeasuring) return;
    if (_activeTraceNumbers.isEmpty()) return;

    // Проверяем, что есть активное подключение
    if (!_socket || _socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "❌ Нет активного подключения для запроса FDAT";
        return;
    }

    QVector<VNAcomand*> fx;
    fx.append(new CALC_TRACE_DATA_XAXIS(_activeTraceNumbers.first()));
    sendCommandImpl(_host, _port, fx);

    for (int tr : _activeTraceNumbers) {
        QVector<VNAcomand*> cmds;
        cmds.append(new CALC_TRACE_SELECT(tr));
        if (_powerMeasuring)
            cmds.append(new CALC_TRACE_DATA_POWER(tr));
        else
            cmds.append(new CALC_TRACE_DATA_FDAT(tr));

        sendCommandImpl(_host, _port, cmds);
    }
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
    cmds.append(new INIT_CONT_MODE(1, true));
    cmds.append(new TRIGGER_SOURCE("IMM"));

    // ОТПРАВЛЯЕМ С ОЖИДАНИЕМ ЗАВЕРШЕНИЯ
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

    _powerMeasuring = false;
    if (_fdatTimer) _fdatTimer->stop();

    QVector<VNAcomand*> cmds;
    cmds.append(new ABORT_COMMAND());
    cmds.append(new INIT_CONT_MODE(1, false));
    sendCommandImpl(_host, _port, cmds);

    qDebug() << "🛑 Измерение мощности остановлено";
}

void Socket::onConnected()
{
    qDebug() << "🔗 Подключение установлено к" << _host.toString() << ":" << _port;
    emit connected();
}

void Socket::onDisconnected()
{
    qDebug() << "🔌 Подключение разорвано";
    emit disconnected();
}

VNAclient* Socket::getInstance()
{
    return this;
}

bool Socket::canConnect(const QString &ip, quint16 port)
{
    qDebug() << "=== ДИАГНОСТИКА ПОДКЛЮЧЕНИЯ ===";
    qDebug() << "Цель:" << ip << ":" << port;

    QTcpSocket testSocket;
    testSocket.connectToHost(ip, port);

    qDebug() << "Состояние сокета после connectToHost:" << testSocket.state();

    bool connected = testSocket.waitForConnected(2000);

    qDebug() << "Состояние сокета после waitForConnected:" << testSocket.state();
    qDebug() << "Результат подключения:" << connected;

    if (!connected) {
        qDebug() << "Код ошибки:" << testSocket.error();
        qDebug() << "Текст ошибки:" << testSocket.errorString();
    }

    if (connected) {
        testSocket.disconnectFromHost();
        if (testSocket.state() != QAbstractSocket::UnconnectedState) {
            testSocket.waitForDisconnected(500);
        }
        qDebug() << "Подключение успешно, сокет отключен";
    }

    qDebug() << "=== КОНЕЦ ДИАГНОСТИКИ ===";
    return connected;
}
