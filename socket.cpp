#include "socket.h"
#include "vnaclient.h"
#include <QDebug>
#include <QThread>

#define TIMEOUT 5000
#define FDAT_INTERVAL 500

Socket::Socket(QObject* parent)
    : VNAclient(parent)
    , _socket(nullptr)
    , _fdatTimer(nullptr)
    , _scanning(false)
    , _powerMeasuring(false)
    , _host(QHostAddress::LocalHost)
    , _port(5025)
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
    qDebug() << "Socket created in thread:" << QThread::currentThread();
}

Socket::~Socket()
{
    stopThread();
}

void Socket::startThread()
{
    if (_thread && !_thread->isRunning()) {
        qDebug() << "Starting Socket thread...";
        _thread->start();
    }
}

void Socket::stopThread()
{
    if (_thread && _thread->isRunning()) {
        qDebug() << "Stopping Socket thread...";
        QMetaObject::invokeMethod(this, "stopScan", Qt::BlockingQueuedConnection);
        QMetaObject::invokeMethod(this, "stopPowerMeasurement", Qt::BlockingQueuedConnection);
        _thread->quit();
        if (!_thread->wait(3000)) {
            qWarning() << "Socket thread didn't finish in time, terminating...";
            _thread->terminate();
            _thread->wait();
        }
    }
}

void Socket::initializeInThread()
{
    qDebug() << "Socket initializing in thread:" << QThread::currentThread();
    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::connected, this, &Socket::onConnected);
    connect(_socket, &QTcpSocket::disconnected, this, &Socket::onDisconnected);

    _fdatTimer = new QTimer(this);
    _fdatTimer->setInterval(FDAT_INTERVAL);
    _fdatTimer->setSingleShot(false);
    connect(_fdatTimer, &QTimer::timeout, this, &Socket::requestFDAT);

    qDebug() << "Socket initialized successfully in thread:" << QThread::currentThread();
}

void Socket::cleanupInThread()
{
    qDebug() << "Socket cleaning up in thread:" << QThread::currentThread();

    if (_fdatTimer) {
        _fdatTimer->stop();
    }

    if (_socket) {
        if (_socket->state() == QAbstractSocket::ConnectedState) {
            _socket->disconnectFromHost();
            if (_socket->state() != QAbstractSocket::UnconnectedState) {
                _socket->waitForDisconnected(1000);
            }
        }
    }

    qDebug() << "Socket cleanup completed";
}

void Socket::setGraphSettings(int graphCount, const QVector<int>& traceNumbers)
{
    _currentGraphCount = graphCount;
    _activeTraceNumbers = traceNumbers;
}

void Socket::sendCommand(const QHostAddress& hostName, quint16 port_,
                         const QVector<VNAcomand*>& commands)
{

    if (QThread::currentThread() != _thread) {
        qWarning() << "sendCommand called from wrong thread! Current:"
                   << QThread::currentThread() << "Expected:" << _thread;
        QVector<VNAcomand*>* commandsCopy = new QVector<VNAcomand*>();
        for (VNAcomand* cmd : commands) {

            commandsCopy->append(new VNAcomand(*cmd));
        }
        QMetaObject::invokeMethod(this, "sendCommand", Qt::QueuedConnection,
                                  Q_ARG(QHostAddress, hostName),
                                  Q_ARG(quint16, port_),
                                  Q_ARG(QVector<VNAcomand*>, *commandsCopy));


        qDeleteAll(*commandsCopy);
        delete commandsCopy;
        return;
    }


    sendCommandImpl(hostName, port_, commands);
}

void Socket::sendCommandImpl(const QHostAddress& hostName, quint16 port_,
                             const QVector<VNAcomand*>& commands)
{
    if (_socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Connecting to VNA from thread:" << QThread::currentThread();
        _socket->connectToHost(hostName, port_);

        if (!_socket->waitForConnected(TIMEOUT)) {
            qDebug() << "Connection FAILED:" << _socket->errorString();
            emit error(_socket->error(), _socket->errorString());
            qDeleteAll(commands);
            return;
        }
        qDebug() << "Connected to VNA successfully";
    }

    for (VNAcomand* cmd : commands) {
        if (!cmd) continue;

        QString cleanScpi = cmd->SCPI.trimmed();
        qDebug() << ">>> SENDING from thread" << QThread::currentThread() << ":" << cleanScpi;

        QByteArray data = cmd->SCPI.toUtf8();
        qint64 bytesWritten = _socket->write(data);

        if (bytesWritten == -1) {
            qDebug() << "Write ERROR:" << _socket->errorString();
            continue;
        }

        _socket->flush();

        if (!cmd->request) {
            delete cmd;
            continue;
        }

        QByteArray responseData;
        int waitTime = 0;
        while (_socket->waitForReadyRead(100) && waitTime < TIMEOUT) {
            responseData += _socket->readAll();
            waitTime += 100;

            if (responseData.contains('\n') || responseData.length() > 1000) {
                break;
            }
        }

        QString responseString = QString::fromUtf8(responseData).trimmed();
        emit dataFromVNA(responseString, cmd);

        QThread::msleep(50);
    }
}
void Socket::startScan(int startKHz, int stopKHz, int points, int band)
{
    if (QThread::currentThread() != _thread) {
        qDebug() << "startScan called from wrong thread, queuing...";
        QMetaObject::invokeMethod(this, "startScan", Qt::QueuedConnection,
                                  Q_ARG(int, startKHz), Q_ARG(int, stopKHz),
                                  Q_ARG(int, points), Q_ARG(int, band));
        return;
    }

    qDebug() << "=== STARTING S-PARAMETER SCAN in thread:" << QThread::currentThread();

    _host = QHostAddress::LocalHost;
    _port = 5025;
    _lastType = 1;

    qint64 startHz = qint64(startKHz) * 1000LL;
    qint64 stopHz = qint64(stopKHz) * 1000LL;
    qint64 bwHz = qint64(band);

    QVector<VNAcomand*> cmds;
    cmds.append(new SENS_FREQ_START(_lastType, startHz));
    cmds.append(new SENS_FREQ_STOP(_lastType, stopHz));
    cmds.append(new SENS_SWE_POINT(_lastType, points));
    cmds.append(new SENS_BWID(_lastType, bwHz));
    cmds.append(new INIT_CONT_MODE(1, true));
    cmds.append(new TRIGGER_SOURCE("IMM"));

    sendCommandImpl(_host, _port, cmds);

    if (!_scanning) {
        _scanning = true;

        if (_fdatTimer) {
            _fdatTimer->start();
            qDebug() << "FDAT timer STARTED in thread:" << QThread::currentThread();
        }

        qDebug() << "S-parameter scanning started successfully";
    }
}

void Socket::stopScan()
{
    if (QThread::currentThread() != _thread) {
        QMetaObject::invokeMethod(this, "stopScan", Qt::QueuedConnection);
        return;
    }

    if (!_scanning) {
        return;
    }

    qDebug() << "=== STOPPING SCAN in thread:" << QThread::currentThread();
    _scanning = false;

    if (_fdatTimer && _fdatTimer->isActive()) {
        _fdatTimer->stop();
        qDebug() << "FDAT timer stopped";
    }

    QVector<VNAcomand*> cmds;
    cmds.append(new VNAcomand(false, 0, ":ABOR\n"));
    cmds.append(new INIT_CONT_MODE(1, false));
    sendCommandImpl(_host, _port, cmds);

    qDebug() << "Scanning stopped";
}

void Socket::requestFDAT()
{
    Q_ASSERT(QThread::currentThread() == _thread);

    if (!_scanning && !_powerMeasuring) {
        return;
    }

    if (_activeTraceNumbers.isEmpty()) {
        return;
    }

    static int requestCount = 0;
    requestCount++;

    if (_powerMeasuring) {
        qDebug() << "=== POWER MEASUREMENT FDAT REQUEST #" << requestCount << " ===";
    } else {
        qDebug() << "=== S-PARAMETER FDAT REQUEST #" << requestCount << " ===";
    }

    // Запрашиваем данные частот для первого графика
    if (!_activeTraceNumbers.isEmpty()) {
        QVector<VNAcomand*> freqCmds;
        freqCmds.append(new CALC_TRACE_DATA_XAXIS(_activeTraceNumbers.first()));
        if (_powerMeasuring) {
            qDebug() << "Requesting frequency data for power measurement";
        }
        sendCommandImpl(_host, _port, freqCmds);
    }

    // Затем запрашиваем амплитудные данные для всех графиков
    for (int traceNum : _activeTraceNumbers) {
        QVector<VNAcomand*> cmds;

        if (_powerMeasuring) {
            cmds.append(new CALC_TRACE_SELECT(traceNum));
            cmds.append(new CALC_TRACE_DATA_POWER(traceNum));
            qDebug() << "Requesting POWER data for trace" << traceNum;
        } else {
            cmds.append(new CALC_TRACE_SELECT(traceNum));
            cmds.append(new CALC_TRACE_DATA_FDAT(traceNum));
            qDebug() << "Requesting S-parameter data for trace" << traceNum;
        }

        sendCommandImpl(_host, _port, cmds);
        QThread::msleep(50);
    }

    if (_powerMeasuring && (requestCount % 10 == 0)) {
        QVector<VNAcomand*> powerCmd;
        powerCmd.append(new SOURCE_POWER_LEVEL_GET(1));
        qDebug() << "Requesting current source power level";
        sendCommandImpl(_host, _port, powerCmd);
    }
}

void Socket::startPowerMeasurement(int startKHz, int stopKHz, int points, int band)
{
    qDebug() << "=== STARTING POWER MEASUREMENT (Receiver Method) ===";
    qDebug() << "Parameters - Start:" << startKHz << "kHz, Stop:" << stopKHz
             << "kHz, Points:" << points << "Band:" << band << "Hz";

    _powerStartKHz = startKHz;
    _powerStopKHz = stopKHz;
    _powerPoints = points;
    _powerBand = band;

    _host = QHostAddress::LocalHost;
    _port = 5025;
    _lastType = 1;

    qint64 startHz = qint64(startKHz) * 1000LL;
    qint64 stopHz = qint64(stopKHz) * 1000LL;
    qint64 bwHz = qint64(band);

    QVector<VNAcomand*> cmds;

    cmds.append(new SYSTEM_PRESET());
    cmds.append(new SOURCE_POWER_COUPLE(1, false));
    cmds.append(new SOURCE_POWER_LEVEL_SET(1, 0.0));
    cmds.append(new OUTPUT_PORT_STATE(1, true));
    cmds.append(new SENS_FREQ_START(_lastType, startHz));
    cmds.append(new SENS_FREQ_STOP(_lastType, stopHz));
    cmds.append(new SENS_SWE_POINT(_lastType, points));
    cmds.append(new SENS_BWID(_lastType, bwHz));
    cmds.append(new CALC_PARAMETER_POWER(1, "R1"));
    cmds.append(new CALC_TRACE_FORMAT_POWER(1, "MLOG"));
    cmds.append(new DISPLAY_WINDOW_ACTIVATE(1));
    cmds.append(new DISP_WIND_TRACE(1, 1));
    cmds.append(new SOURCE_POWER_LEVEL_GET(1));
    cmds.append(new INIT_CONT_MODE(1, true));
    cmds.append(new TRIGGER_SOURCE("IMM"));

    sendCommandImpl(_host, _port, cmds);

    if (!_powerMeasuring) {
        _powerMeasuring = true;

        QVector<int> powerTraces;
        powerTraces.append(1);
        setGraphSettings(1, powerTraces);

        if (_fdatTimer) {
            _fdatTimer->start();
            qDebug() << "Power measurement FDAT timer STARTED";
        }

        qDebug() << "Power measurement (receiver method) started successfully";
    }
}

void Socket::stopPowerMeasurement()
{
    if (!_powerMeasuring) {
        qDebug() << "StopPowerMeasurement: power measurement was not active";
        return;
    }

    qDebug() << "=== STOPPING POWER MEASUREMENT ===";
    _powerMeasuring = false;

    if (_fdatTimer && _fdatTimer->isActive()) {
        _fdatTimer->stop();
        qDebug() << "Power measurement FDAT timer stopped";
    }

    QVector<VNAcomand*> cmds;
    cmds.append(new VNAcomand(false, 0, ":ABOR\n"));
    cmds.append(new INIT_CONT_MODE(1, false));
    cmds.append(new SOURCE_POWER_COUPLE(1, true));
    cmds.append(new OUTPUT_PORT_STATE(1, false));

    sendCommandImpl(_host, _port, cmds);

    qDebug() << "Power measurement stopped";
}

void Socket::onConnected()
{
    qDebug() << "Socket connected to VNA in thread:" << QThread::currentThreadId();
    emit connected();
}

void Socket::onDisconnected()
{
    qDebug() << "Socket disconnected from VNA in thread:" << QThread::currentThreadId();
    emit disconnected();
}


VNAclient* Socket::getInstance()
{
    return this;
}
