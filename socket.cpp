#include "socket.h"
#include "vnaclient.h"
#include <QDebug>
#include <QThread>
#include <QTimer>

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
{
    _socket = new QTcpSocket(this);

    _fdatTimer = new QTimer(this);
    _fdatTimer->setInterval(FDAT_INTERVAL);
    _fdatTimer->setSingleShot(false);
    connect(_socket, &QTcpSocket::connected, this, &Socket::onConnected);
    connect(_socket, &QTcpSocket::disconnected, this, &Socket::onDisconnected);
    connect(_fdatTimer, &QTimer::timeout, this, &Socket::requestFDAT);
}

Socket::~Socket()
{
    stopScan();
    stopPowerMeasurement();

    if (_fdatTimer)
    {
        _fdatTimer->stop();
        _fdatTimer->deleteLater();
    }

    if (_socket)
    {
        if (_socket->isOpen())
        {
            _socket->disconnectFromHost();
            if (_socket->state() != QAbstractSocket::UnconnectedState)
            {
                _socket->waitForDisconnected(1000);
            }
        }
        _socket->deleteLater();
    }
}

void Socket::setGraphSettings(int graphCount, const QVector<int>& traceNumbers)
{
    _currentGraphCount = graphCount;
    _activeTraceNumbers = traceNumbers;
}

void Socket::sendCommand(const QHostAddress& hostName, quint16 port_, const QVector<VNAcomand*>& commands)
{
    if (_socket->state() != QAbstractSocket::ConnectedState)
    {
        _socket->connectToHost(hostName, port_);

        if (!_socket->waitForConnected(TIMEOUT))
        {
            emit error(_socket->error(), _socket->errorString());
            qDeleteAll(commands);
            return;
        }
    }

    for (VNAcomand* cmd : commands)
    {
        if (!cmd) continue;

        QByteArray data = cmd->SCPI.toUtf8();
        qint64 bytesWritten = _socket->write(data);

        if (bytesWritten == -1)
        {
            continue;
        }

        _socket->flush();

        if (!cmd->request)
        {
            delete cmd;
            continue;
        }

        QByteArray responseData;
        int waitTime = 0;
        while (_socket->waitForReadyRead(100) && waitTime < TIMEOUT)
        {
            responseData += _socket->readAll();
            waitTime += 100;

            if (responseData.contains('\n') || responseData.length() > 1000)
            {
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

    sendCommand(_host, _port, cmds);

    if (!_scanning)
    {
        _scanning = true;

        if (_fdatTimer)
        {
            _fdatTimer->start();
        }
    }
}

void Socket::stopScan()
{
    if (!_scanning)
    {
        return;
    }

    _scanning = false;

    if (_fdatTimer && _fdatTimer->isActive())
    {
        _fdatTimer->stop();
    }

    QVector<VNAcomand*> cmds;
    cmds.append(new VNAcomand(false, 0, ":ABOR\n"));
    cmds.append(new INIT_CONT_MODE(1, false));
    sendCommand(_host, _port, cmds);
}

void Socket::requestFDAT()
{
    if (!_scanning && !_powerMeasuring)
    {
        return;
    }

    if (_activeTraceNumbers.isEmpty())
    {
        return;
    }

    static int requestCount = 0;
    requestCount++;

    if (!_activeTraceNumbers.isEmpty())
    {
        QVector<VNAcomand*> freqCmds;
        freqCmds.append(new CALC_TRACE_DATA_XAXIS(_activeTraceNumbers.first()));
        sendCommand(_host, _port, freqCmds);
    }

    for (int traceNum : _activeTraceNumbers)
    {
        QVector<VNAcomand*> cmds;

        if (_powerMeasuring)
        {
            cmds.append(new CALC_TRACE_SELECT(traceNum));
            cmds.append(new CALC_TRACE_DATA_POWER(traceNum));
        }
        else
        {
            cmds.append(new CALC_TRACE_SELECT(traceNum));
            cmds.append(new CALC_TRACE_DATA_FDAT(traceNum));
        }

        sendCommand(_host, _port, cmds);
        QThread::msleep(50);
    }

    if (_powerMeasuring && (requestCount % 10 == 0))
    {
        QVector<VNAcomand*> powerCmd;
        powerCmd.append(new SOURCE_POWER_LEVEL_GET(1));
        sendCommand(_host, _port, powerCmd);
    }
}

void Socket::startPowerMeasurement(int startKHz, int stopKHz, int points, int band)
{
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

    sendCommand(_host, _port, cmds);

    if (!_powerMeasuring)
    {
        _powerMeasuring = true;

        QVector<int> powerTraces;
        powerTraces.append(1);
        setGraphSettings(1, powerTraces);

        if (_fdatTimer)
        {
            _fdatTimer->start();
        }
    }
}

void Socket::stopPowerMeasurement()
{
    if (!_powerMeasuring)
    {
        return;
    }

    _powerMeasuring = false;

    if (_fdatTimer && _fdatTimer->isActive())
    {
        _fdatTimer->stop();
    }

    QVector<VNAcomand*> cmds;
    cmds.append(new VNAcomand(false, 0, ":ABOR\n"));
    cmds.append(new INIT_CONT_MODE(1, false));
    cmds.append(new SOURCE_POWER_COUPLE(1, true));
    cmds.append(new OUTPUT_PORT_STATE(1, false));

    sendCommand(_host, _port, cmds);
}

void Socket::onConnected()
{
    emit connected();
}

void Socket::onDisconnected()
{
    emit disconnected();
}

VNAclient* Socket::getInstance()
{
    return this;
}
