#include "widget.h"
#include "socket.h"
#include <QHBoxLayout>
#include <QQmlContext>
#include <QQuickWidget>
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QChartView>
#include <QApplication>

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
                                              {"Мним", "IMAG"},
                                              };
    return unitMap.value(unit, "MLOG");
}

Widget::Widget(VNAclient* client, QWidget* parent)
    : QWidget(parent)
    , _vnaClient(client)
    , _chartManager(nullptr)
    , _chartView(nullptr)
    , _currentIP("127.0.0.1")
    , _currentPort(5025)
    , _currentStartKHz(20)
    , _currentStopKHz(4800000)
    , _currentPoints(201)
    , _currentBand(10000)
    ,_currentPowerDbM(0.0)
{
    _chartManager = new CreaterChart(this);
    setupUi();
    qRegisterMetaType<QVector<VNAcomand*>>();
    qRegisterMetaType<QHostAddress>();
    connect(_vnaClient, &VNAclient::dataFromVNA, this, &Widget::dataFromVNA, Qt::QueuedConnection);
    connect(_vnaClient, &VNAclient::error, this, &Widget::errorMessage, Qt::QueuedConnection);
    setOptimalScanSettings();
    startSocketThread();
}

Widget::~Widget()
{
    stopSocketThread();
    if (_vnaClient) {
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

void Widget::setOptimalScanSettings()
{
    Socket* socket = qobject_cast<Socket*>(_vnaClient);
    if (socket) {
        socket->setTimeouts(20000, 60000, 2000);
    }
}

void Widget::startScanFromQml(const QString& ip, quint16 port, int startKHz, int stopKHz, int points, int band, double powerDbM)
{
    if (!_vnaClient) return;
    QHostAddress addr;
    if (!addr.setAddress(ip)) {
        showIpPortError(QString("Некорректный IP: %1").arg(ip));
        return;
    }
    if (port < 1 || port > 65535) {
        showIpPortError(QString("Некорректный порт: %1").arg(port));
        return;
    }
    _currentIP = ip;
    _currentPort = port;
    _currentStartKHz = startKHz;
    _currentStopKHz = stopKHz;
    _currentPoints = points;
    _currentBand = band;
    _currentPowerDbM = powerDbM;
    Socket* socket = qobject_cast<Socket*>(_vnaClient);
    if (socket && !socket->canConnect(ip, port)) {
        showIpPortError("Прибор недоступен по указанному IP/порт");
        return;
    }
    QMetaObject::invokeMethod(
        _vnaClient, "startScan", Qt::QueuedConnection,
        Q_ARG(QString, ip),
        Q_ARG(quint16, port),
        Q_ARG(int, startKHz),
        Q_ARG(int, stopKHz),
        Q_ARG(int, points),
        Q_ARG(int, band),
        Q_ARG(double, powerDbM)
        );
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
    int startFreq = params.value("startFreq").toInt();
    int stopFreq = params.value("stopFreq").toInt();
    int points = params.value("numberOfPoints").toInt();
    int band = params.value("freqBand").toInt();
    QString sweepType = params.value("sweepType").toString(); // тип сканирования
    qDebug() << "Applying graph settings with sweep type:" << sweepType;
    _chartManager->clearAllTraces();
    QVector<VNAcomand*> cmds;
    QVector<int> traceNumbers;
    int graphCount = graphs.size();
    cmds.append(new SENSE_SWEEP_TYPE(1, sweepType));
    cmds.append(new CALC_PARAMETER_COUNT(graphCount));
    for (const QVariant& v : graphs) {
        QVariantMap g = v.toMap();
        int num = g.value("num").toInt();
        QString type = g.value("type").toString();
        QString unit = g.value("unit").toString();
        int port = g.value("port", 0).toInt();
        QString scpiUnit = unitToScpi(unit);
        QColor traceColor = QColor::fromHsv((num * 40) % 360, 200, 200);
        QString traceName;
        if (port > 0) {
            traceName = QString("Trace %1 (%2(%3))").arg(num).arg(type).arg(port);
        } else {
            traceName = QString("Trace %1 (%2)").arg(num).arg(type);
        }
        _chartManager->addTrace(num, traceName, traceColor);
        traceNumbers.append(num);
        cmds.append(new CALC_PARAMETER_DEFINE(num, type));
        if (port > 0) {
            cmds.append(new CALC_PARAMETER_SPORT(num, port));
        }
        cmds.append(new CALC_TRACE_SELECT(num));
        cmds.append(new CALC_TRACE_FORMAT(num, scpiUnit));
        cmds.append(new DISP_WIND_TRACE(1, num));
    }
    cmds.append(new OPC_QUERY());
    if (_vnaClient && !cmds.isEmpty()) {
        QVector<VNAcomand*> cmdsCopy = cmds;
        QMetaObject::invokeMethod(_vnaClient, "setGraphSettings", Qt::QueuedConnection,
                                  Q_ARG(int, graphCount),
                                  Q_ARG(QVector<int>, traceNumbers));
        QHostAddress targetHost;
        if (!_currentIP.isEmpty() && targetHost.setAddress(_currentIP)) {
            QMetaObject::invokeMethod(_vnaClient, "sendCommand", Qt::QueuedConnection,
                                      Q_ARG(QHostAddress, targetHost),
                                      Q_ARG(quint16, _currentPort),
                                      Q_ARG(QVector<VNAcomand*>, cmdsCopy));
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
    }
}

void Widget::stopSocketThread()
{
    Socket* socket = qobject_cast<Socket*>(_vnaClient);
    if (socket) {
        socket->stopThread();
    }
}

void Widget::dataFromVNA(const QString& data, VNAcomand* cmd)
{
    if (!cmd) return;
    if (auto* xaxisCmd = dynamic_cast<CALC_TRACE_DATA_XAXIS*>(cmd)) {
        _frequencyData = xaxisCmd->parseResponse(data);
    } else if (auto* traceCmd = dynamic_cast<CALC_TRACE_DATA_FDAT*>(cmd)) {
        QVector<qreal> amplitudeData = traceCmd->parseResponse(data);
        QVector<qreal> xData;
        if (!_frequencyData.isEmpty() && _frequencyData.size() == amplitudeData.size()) {
            xData = _frequencyData;
        } else {
            for (int i = 0; i < amplitudeData.size(); ++i) {
                xData.append(i);
            }
        }
        int traceNum = traceCmd->type;
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
        port = 5025;
    }
    QHostAddress testAddr;
    if (!testAddr.setAddress(ip) || ip.split('.').length() != 4) {
        return;
    }
    _currentIP = ip;
    _currentPort = port;
}
