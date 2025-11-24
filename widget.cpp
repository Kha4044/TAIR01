#include "widget.h"
#include "socket.h"
#include <QHBoxLayout>
#include <QQmlContext>
#include <QQuickWidget>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QChartView>
#include <QRandomGenerator>
#include <QtMath>

static QString unitToScpi(const QString& unit)
{
    static QHash<QString, QString> unitMap = {
        {"Амп.лог", "MLOG"},
        {"КСВН", "SWR"},
        {"Фаза", "PHAS"},
        {"Фаза>180", "UPHase"},
        {"ГВЗ", "GDEL"},
        {"Амп лин", "MLIN"},
        {"Реал", "REAL"},
        {"Мним", "IMAG"}
    };
    return unitMap.value(unit, "MLOG");
}

Widget::Widget(VNAclient* client, QWidget* parent)
    : QWidget(parent)
    , _vnaClient(client)
    , _chartManager(nullptr)
    , _chartView(nullptr)
    , _powerMeasuringMode(false)
    , _currentIP("192.168.0.1")  // Значения по умолчанию
    , _currentPort(5025)
    , _currentStartKHz(20)
    , _currentStopKHz(4800000)
    , _currentPoints(201)
    , _currentBand(10000)
{
    _chartManager = new CreaterChart(this);
    setupUi();

    qRegisterMetaType<QVector<VNAcomand*>>();
    qRegisterMetaType<QHostAddress>();
    qRegisterMetaType<QAbstractSocket::SocketError>();

    connect(_vnaClient, &VNAclient::dataFromVNA, this, &Widget::dataFromVNA, Qt::QueuedConnection);
    connect(_vnaClient, &VNAclient::error, this, &Widget::errorMessage, Qt::QueuedConnection);

    startSocketThread();
    QTimer::singleShot(1000, this, &Widget::addTestData);
}

Widget::~Widget()
{
    stopSocketThread();
    if (_vnaClient) {
        qDebug() << "Остановка клиента VNA...";
        if (_powerMeasuringMode) {
            cleanupPowerMeasurement();
        }
        QMetaObject::invokeMethod(_vnaClient, "stopScan", Qt::BlockingQueuedConnection);
    }
}

void Widget::setupUi()
{
    QHBoxLayout* lay = new QHBoxLayout(this);

    QQuickWidget* qw = new QQuickWidget(this);
    qw->rootContext()->setContextProperty("mainWidget", this);
    qw->rootContext()->setContextProperty("vnaClient", _vnaClient);
    qw->setSource(QUrl(QStringLiteral("qrc:/widget.qml")));
    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qw->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    qw->setMinimumWidth(435);
    qw->setMaximumWidth(435);

    _chartManager->setupAxes("Frequency (kHz)", "Amplitude");
    _chartView = new QChartView(_chartManager->getChart());
    _chartView->setMinimumSize(800, 640);
    _chartView->setRenderHint(QPainter::Antialiasing);

    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(qw, 2);
    lay->addWidget(_chartView, 4);
}

void Widget::addTestData()
{
    _chartManager->addTrace(99, "Test Line", Qt::red);
    QVector<qreal> xData = {0, 10, 20, 30, 40, 50};
    QVector<qreal> yData = {10, 25, 15, 30, 20, 35};
    _chartManager->updateTraceData(99, xData, yData);
    _chartManager->autoScaleAxes();
    _chartView->update();
}

void Widget::requestFrequencyData()
{
    if (!_vnaClient) return;

    int firstTraceNum = 1;
    QVector<VNAcomand*> cmds;
    cmds.append(new CALC_TRACE_DATA_XAXIS(firstTraceNum));

    // ИСПОЛЬЗУЕМ СОХРАНЕННЫЕ НАСТРОЙКИ ПОДКЛЮЧЕНИЯ
    QHostAddress targetHost;
    if (!_currentIP.isEmpty() && targetHost.setAddress(_currentIP)) {
        QMetaObject::invokeMethod(_vnaClient, "sendCommand", Qt::QueuedConnection,
                                  Q_ARG(QHostAddress, targetHost),
                                  Q_ARG(quint16, _currentPort),
                                  Q_ARG(QVector<VNAcomand*>, cmds));
    } else {
        // Fallback если нет сохраненных настроек
        QMetaObject::invokeMethod(_vnaClient, "sendCommand", Qt::QueuedConnection,
                                  Q_ARG(QHostAddress, QHostAddress::LocalHost),
                                  Q_ARG(quint16, 5025),
                                  Q_ARG(QVector<VNAcomand*>, cmds));
    }
}

void Widget::startScanFromQml(const QString& ip, quint16 port, int startKHz, int stopKHz, int points, int band)
{
    if (!_vnaClient) {
        qDebug() << "❌ VNA client не инициализирован";
        return;
    }

    qDebug() << "=== ЗАПУСК СКАНИРОВАНИЯ ИЗ QML ===";
    qDebug() << "IP:" << ip << "Port:" << port;

    // Базовая валидация
    QHostAddress addr;
    if (!addr.setAddress(ip)) {
        QString errorMsg = QString("Некорректный IP: %1").arg(ip);
        qDebug() << "❌" << errorMsg;
        showIpPortError(errorMsg);
        return;
    }

    if (port < 1 || port > 65535) {
        QString errorMsg = QString("Некорректный порт: %1").arg(port);
        qDebug() << "❌" << errorMsg;
        showIpPortError(errorMsg);
        return;
    }

    // СОХРАНЯЕМ НАСТРОЙКИ ДЛЯ ДАЛЬНЕЙШЕГО ИСПОЛЬЗОВАНИЯ
    _currentIP = ip;
    _currentPort = port;
    _currentStartKHz = startKHz;
    _currentStopKHz = stopKHz;
    _currentPoints = points;
    _currentBand = band;

    Socket* socket = qobject_cast<Socket*>(_vnaClient);
    if (socket) {
        bool canConnect = socket->canConnect(ip, port);
        qDebug() << "Результат проверки подключения:" << canConnect;

        if (!canConnect) {
            showIpPortError("Прибор недоступен по указанному IP/порт");
            return;
        }
    } else {
        qDebug() << "Не удалось подключить vnaClient к Socket";
    }

    // Запускаем сканирование
    QMetaObject::invokeMethod(
        _vnaClient, "startScan", Qt::QueuedConnection,
        Q_ARG(QString, ip),
        Q_ARG(quint16, port),
        Q_ARG(int, startKHz),
        Q_ARG(int, stopKHz),
        Q_ARG(int, points),
        Q_ARG(int, band)
        );

    qDebug() << "✅ Команда startScan отправлена в поток сокета";
}

void Widget::stopScanFromQml(const QString& ip, int port)
{
    Q_UNUSED(ip)
    Q_UNUSED(port)

    if (!_vnaClient) return;

    QMetaObject::invokeMethod(_vnaClient, "stopScan", Qt::QueuedConnection);
}

void Widget::applyGraphSettings(const QVariantList& graphs, const QVariantMap& params)
{
    Q_UNUSED(params);

    if (_powerMeasuringMode) {
        QMessageBox::information(this, "Information", "Please exit power measurement mode first to change graph settings.");
        return;
    }

    _chartManager->clearAllTraces();
    QVector<VNAcomand*> cmds;
    QVector<int> traceNumbers;
    int graphCount = graphs.size();

    cmds.append(new CALC_PARAMETER_COUNT(graphCount));

    for (const QVariant& v : graphs) {
        QVariantMap g = v.toMap();
        int num = g.value("num").toInt();
        QString type = g.value("type").toString();
        QString unit = g.value("unit").toString();
        QString scpiUnit = unitToScpi(unit);

        QColor traceColor = QColor::fromHsv((num * 40) % 360, 200, 200);
        QString traceName = QString("Trace %1 (%2)").arg(num).arg(type);
        _chartManager->addTrace(num, traceName, traceColor);
        traceNumbers.append(num);

        cmds.append(new CALC_PARAMETER_DEFINE(num, type));
        cmds.append(new CALC_TRACE_SELECT(num));
        cmds.append(new CALC_TRACE_FORMAT(num, scpiUnit));
        cmds.append(new DISP_WIND_TRACE(1, num));
    }

    // ДОБАВЛЯЕМ OPC ДЛЯ ГАРАНТИИ ЗАВЕРШЕНИЯ НАСТРОЙКИ
    cmds.append(new OPC_QUERY());

    if (_vnaClient && !cmds.isEmpty()) {
        QVector<VNAcomand*> cmdsCopy = cmds;
        QMetaObject::invokeMethod(_vnaClient, "setGraphSettings", Qt::QueuedConnection,
                                  Q_ARG(int, graphCount),
                                  Q_ARG(QVector<int>, traceNumbers));

        // ИСПОЛЬЗУЕМ СОХРАНЕННЫЕ НАСТРОЙКИ ПОДКЛЮЧЕНИЯ
        QHostAddress targetHost;
        if (!_currentIP.isEmpty() && targetHost.setAddress(_currentIP)) {
            QMetaObject::invokeMethod(_vnaClient, "sendCommand", Qt::QueuedConnection,
                                      Q_ARG(QHostAddress, targetHost),
                                      Q_ARG(quint16, _currentPort),
                                      Q_ARG(QVector<VNAcomand*>, cmdsCopy));
        } else {
            QMetaObject::invokeMethod(_vnaClient, "sendCommand", Qt::QueuedConnection,
                                      Q_ARG(QHostAddress, QHostAddress("127.0.0.1")),
                                      Q_ARG(quint16, _currentPort),
                                      Q_ARG(QVector<VNAcomand*>, cmdsCopy));
        }

        if (!traceNumbers.isEmpty()) {
            // Ждем дольше для гарантии применения настроек
            QTimer::singleShot(1500, this, &Widget::requestFrequencyData);
        }
    } else {
        qDeleteAll(cmds);
    }
}

void Widget::startSocketThread()
{
    Socket* socket = qobject_cast<Socket*>(_vnaClient);
    if (socket) {
        socket->startThread();
        qDebug() << "Socket thread started";
    }
}

void Widget::stopSocketThread()
{
    Socket* socket = qobject_cast<Socket*>(_vnaClient);
    if (socket) {
        socket->stopThread();
        qDebug() << "Socket thread stopped";
    }
}

void Widget::setPowerMeasuringMode(bool enabled)
{
    if (_powerMeasuringMode == enabled) {
        return;
    }

    if (enabled) {
        int startKHz = 20;
        int stopKHz = 4800000;
        int points = 201;
        int band = 10000;
        setupPowerMeasurement(startKHz, stopKHz, points, band);
    } else {
        cleanupPowerMeasurement();
    }
}

void Widget::setupPowerMeasurement(int startKHz, int stopKHz, int points, int band)
{
    _powerMeasuringMode = true;
    // stopScanFromQml();
    _powerFrequencyData.clear();
    _powerValueData.clear();
    _chartManager->clearAllTraces();
    _chartManager->setupAxes("Frequency (kHz)", "Power (dBm)");

    if (_chartManager->hasTrace(1000)) {
        _chartManager->removeTrace(1000);
    }
    _chartManager->addTrace(1000, "Source Power (dBm)", QColor("#ff9500"));

    Socket* socketClient = qobject_cast<Socket*>(_vnaClient);
    if (socketClient) {
        QMetaObject::invokeMethod(socketClient, "startPowerMeasurement", Qt::QueuedConnection,
                                  Q_ARG(int, startKHz),
                                  Q_ARG(int, stopKHz),
                                  Q_ARG(int, points),
                                  Q_ARG(int, band));
    }
}

void Widget::cleanupPowerMeasurement()
{
    _powerMeasuringMode = false;
    Socket* socketClient = qobject_cast<Socket*>(_vnaClient);
    if (socketClient) {
        QMetaObject::invokeMethod(socketClient, "stopPowerMeasurement", Qt::QueuedConnection);
    }

    _powerFrequencyData.clear();
    _powerValueData.clear();
    _chartManager->clearAllTraces();
    _chartManager->setupAxes("Frequency (kHz)", "Amplitude");
}

void Widget::dataFromVNA(const QString& data, VNAcomand* cmd)
{
    if (!cmd) {
        return;
    }

    if (auto* powerCmd = dynamic_cast<SOURCE_POWER_LEVEL_GET*>(cmd)) {
        QVector<qreal> powerData = powerCmd->parseResponse(data);
    } else if (auto* xaxisCmd = dynamic_cast<CALC_TRACE_DATA_XAXIS*>(cmd)) {
        int traceNum = xaxisCmd->type;
        _frequencyData = xaxisCmd->parseResponse(data);
    } else if (auto* powerDataCmd = dynamic_cast<CALC_TRACE_DATA_POWER*>(cmd)) {
        int traceNum = powerDataCmd->type;
        QVector<qreal> powerData = powerDataCmd->parseResponse(data);

        if (_powerMeasuringMode && traceNum == 1) {
            QVector<qreal> xData;
            if (!_frequencyData.isEmpty() && _frequencyData.size() == powerData.size()) {
                xData = _frequencyData;
            } else if (!_frequencyData.isEmpty()) {
                for (int i = 0; i < powerData.size(); ++i) {
                    xData.append(i);
                }
            } else {
                for (int i = 0; i < powerData.size(); ++i) {
                    xData.append(i);
                }
            }

            if (!_chartManager->hasTrace(1000)) {
                _chartManager->addTrace(1000, "Source Power (dBm)", QColor("#ff9500"));
            }
            _chartManager->updateTraceData(1000, xData, powerData);
            _chartManager->autoScaleAxes();
            _chartView->update();
        }
    } else if (auto* traceCmd = dynamic_cast<CALC_TRACE_DATA_FDAT*>(cmd)) {
        int traceNum = traceCmd->type;
        if (_powerMeasuringMode) {
            delete cmd;
            return;
        }

        QVector<qreal> amplitudeData = traceCmd->parseResponse(data);
        QVector<qreal> xData;
        if (!_frequencyData.isEmpty() && _frequencyData.size() == amplitudeData.size()) {
            xData = _frequencyData;
        } else {
            for (int i = 0; i < amplitudeData.size(); ++i) {
                xData.append(i);
            }
        }

        if (!_chartManager->hasTrace(traceNum)) {
            QColor traceColor = QColor::fromHsv((traceNum * 40) % 360, 200, 200);
            _chartManager->addTrace(traceNum, QString("Trace %1").arg(traceNum), traceColor);
        }
        _chartManager->updateTraceData(traceNum, xData, amplitudeData);
        _chartManager->autoScaleAxes();
        _chartView->update();
    }

    delete cmd;
}

void Widget::forceDataSync()
{
    if (_powerMeasuringMode) {
        if (_chartView) {
            _chartView->update();
        }
    } else {
        requestFrequencyData();
        if (_chartView) {
            _chartView->update();
        }
    }
}

void Widget::errorMessage(int code, const QString& message)
{
    QMessageBox::warning(this, "VNA Error", QString("Code: %1\nMessage: %2").arg(code).arg(message));
}

void Widget::showIpPortError(const QString &msg)
{
    QMessageBox::warning(this, "IP/Port Error", msg);
}

void Widget::updateConnectionSettings(const QString& ip, quint16 port)
{

    if (port < 1 || port > 65535) {
        qDebug() << "⚠️ Некорректный порт:" << port << ", используем 5025";
        port = 5025;
    }


    QHostAddress testAddr;
    if (!testAddr.setAddress(ip) || ip.split('.').length() != 4) {
        return;
    }

    _currentIP = ip;
    _currentPort = port;
    qDebug() << "✅ Настройки подключения обновлены:" << _currentIP << ":" << _currentPort;
}
