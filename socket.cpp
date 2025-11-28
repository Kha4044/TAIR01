#include "socket.h"
#include "vnaclient.h"
#include "vnacomand.h"
#include <QDebug>
#include <QThread>
#include <QEventLoop>
#include <QTimer>

#define DEFAULT_NORMAL_TIMEOUT_MS 15000
#define DEFAULT_OPC_TIMEOUT_MS  45000
#define DEFAULT_FDAT_INTERVAL_MS 2000

Socket::Socket(QObject* parent)
    : VNAclient(parent)
    , _socket(nullptr)
    , _fdatTimer(nullptr)
    , _thread(nullptr)
    , _scanning(false)
    , _currentGraphCount(1)
    , _normalTimeout(DEFAULT_NORMAL_TIMEOUT_MS)
    , _opcTimeout(DEFAULT_OPC_TIMEOUT_MS)
    , _fdatInterval(DEFAULT_FDAT_INTERVAL_MS)
    , _host(QHostAddress::LocalHost)
    , _port(5025)
{
    _thread = new QThread();
    this->moveToThread(_thread);
    connect(_thread, &QThread::started, this, &Socket::initializeInThread);
    connect(_thread, &QThread::finished, this, &Socket::cleanupInThread);
}

Socket::~Socket()
{
    stopThread();
    if (_thread) {
        delete _thread;
        _thread = nullptr;
    }
}

VNAclient* Socket::getInstance()
{
    return this;
}

void Socket::setTimeouts(int normalTimeoutMs, int opcTimeoutMs, int fdatIntervalMs)
{
    _normalTimeout = normalTimeoutMs;
    _opcTimeout = opcTimeoutMs;
    _fdatInterval = fdatIntervalMs;
    if (_fdatTimer) {
        QMetaObject::invokeMethod(_fdatTimer, "setInterval", Qt::QueuedConnection, Q_ARG(int, _fdatInterval));
    }
}

void Socket::startThread()
{
    if (_thread && !_thread->isRunning()) {
        _thread->start();
    }
}
void Socket::stopThread()
{
    if (!_thread) return;
    if (_thread->isRunning()) {
        _scanning = false;
        QMetaObject::invokeMethod(this, "stopInThread", Qt::BlockingQueuedConnection);
        _thread->quit();
        if (!_thread->wait(3000)) {
            qWarning() << "Socket thread didn't finish in time; terminating.";
            _thread->terminate();
            _thread->wait();
        }
    }
}

void Socket::initializeInThread()
{
    qDebug() << "Socket::initializeInThread in thread" << QThread::currentThread();
    _socket = new QTcpSocket(this);
    connect(_socket, &QTcpSocket::connected, this, &Socket::onConnected);
    connect(_socket, &QTcpSocket::disconnected, this, &Socket::onDisconnected);
    connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, [this](QAbstractSocket::SocketError err){
                if (!_scanning) return;
                qWarning() << "Socket error:" << err << _socket->errorString();
                emit error(err, _socket->errorString());
            });
    _fdatTimer = new QTimer(this);
    _fdatTimer->setInterval(_fdatInterval);
    _fdatTimer->setSingleShot(false);
    connect(_fdatTimer, &QTimer::timeout, this, &Socket::requestFDAT);
    qDebug() << "Socket initialized (thread): timeouts normal=" << _normalTimeout
             << " opc=" << _opcTimeout << " fdatInterval=" << _fdatInterval;
}

void Socket::cleanupInThread()
{
    qDebug() << "Socket::cleanupInThread in thread" << QThread::currentThread();
    if (_fdatTimer) {
        if (_fdatTimer->isActive()) _fdatTimer->stop();
        _fdatTimer = nullptr;
    }
    if (_socket) {
        if (_socket->state() != QAbstractSocket::UnconnectedState) {
            _socket->disconnectFromHost();
            if (_socket->state() != QAbstractSocket::UnconnectedState) {
                _socket->waitForDisconnected(1000);
            }
        }
        _socket = nullptr;
    }
    qDebug() << "Socket cleanup finished (cleanupInThread)";
}
void Socket::stopInThread()
{
    qDebug() << "Socket::stopInThread() executing in thread" << QThread::currentThread();
    if (_fdatTimer) {
        if (_fdatTimer->isActive()) _fdatTimer->stop();
    }
    if (_socket) {
        if (_socket->state() != QAbstractSocket::UnconnectedState) {
            _socket->disconnectFromHost();
            if (_socket->state() != QAbstractSocket::UnconnectedState) {
                _socket->waitForDisconnected(1000);
            }
        }
    }
    _scanning = false;
    qDebug() << "Socket::stopInThread completed";
}

bool Socket::ensureConnection(const QHostAddress& host, quint16 port)
{
    if (!_socket) {
        qWarning() << "Socket not initialized (ensureConnection)";
        return false;
    }
    if (_socket->state() == QAbstractSocket::ConnectedState && _host == host && _port == port) {
        return true;
    }
    if (_socket->state() == QAbstractSocket::ConnectedState) {
        _socket->disconnectFromHost();
        if (_socket->state() != QAbstractSocket::UnconnectedState) {
            _socket->waitForDisconnected(1000);
        }
    }
    qDebug() << "Connecting to" << host.toString() << port;
    _socket->connectToHost(host, port);
    if (!_socket->waitForConnected(_normalTimeout)) {
        qWarning() << "Failed to connect within" << _normalTimeout << "ms. err:" << _socket->errorString();
        emit error(_socket->error(), _socket->errorString());
        return false;
    }
    _host = host;
    _port = port;
    QThread::msleep(50);
    qDebug() << "Connected to" << _host.toString() << ":" << _port;
    return true;
}

bool Socket::waitForOperationsComplete(int timeoutMs)
{
    if (!_socket || _socket->state() != QAbstractSocket::ConnectedState) {
        qWarning() << "No socket or not connected for OPC";
        return false;
    }
    _socket->readAll();
    qDebug() << "Sending *OPC? and waiting up to" << timeoutMs << "ms";
    _socket->write("*OPC?\n");
    _socket->flush();
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(_socket, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();
    if (!timer.isActive()) {
        qWarning() << "OPC wait timeout";
        return false;
    }
    QByteArray resp = _socket->readAll().trimmed();
    qDebug() << "OPC response raw:" << resp;
    return (resp == "1" || resp == "1\r" || resp == "1\n");
}

void Socket::sendCommandWithOPC(const QHostAddress& host, quint16 port, const QVector<VNAcomand*>& commands)
{
    if (!ensureConnection(host, port)) {
        qDeleteAll(commands);
        return;
    }
    for (auto *cmd : commands) {
        QByteArray ba = cmd->SCPI.toUtf8();
        qDebug() << "sendCommandWithOPC: write:" << ba.trimmed();
        _socket->write(ba);
        _socket->flush();
        if (cmd->request) {
            if (!_socket->waitForReadyRead(_normalTimeout)) {
                qWarning() << "Timeout waiting reply for" << ba.trimmed();
                emit error(-1, QString("Timeout for command: %1").arg(QString::fromUtf8(ba)));
                delete cmd;
                continue;
            }
            QByteArray reply = _socket->readAll();
            emit dataFromVNA(QString::fromUtf8(reply), cmd);
        } else {
            delete cmd;
        }
    }
    if (!waitForOperationsComplete(_opcTimeout)) {
        qWarning() << "OPC did not complete within" << _opcTimeout << "ms";
    }
}

void Socket::sendCommand(const QHostAddress& host, quint16 port, const QVector<VNAcomand*>& commands)
{
    if (QThread::currentThread() != _thread) {
        QVector<VNAcomand*> *copy = new QVector<VNAcomand*>();
        copy->reserve(commands.size());
        for (auto *c : commands) {
            copy->append(new VNAcomand(c->request, c->type, c->SCPI));
        }
        QMetaObject::invokeMethod(this, "sendCommand", Qt::QueuedConnection,
                                  Q_ARG(QHostAddress, host),
                                  Q_ARG(quint16, port),
                                  Q_ARG(QVector<VNAcomand*>, *copy));
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
        qDebug() << "sendCommandImpl: sending" << ba.trimmed();
        _socket->write(ba);
        _socket->flush();
        if (!cmd->request) {
            delete cmd;
            continue;
        }
        int timeout = _normalTimeout;
        if (cmd->SCPI.contains("FDAT") || cmd->SCPI.contains("XAXIS")) {
            timeout = qMax(timeout, 30000);
        }
        if (!_socket->waitForReadyRead(timeout)) {
            qWarning() << "Timeout waiting response to" << ba.trimmed() << "timeout(ms)=" << timeout;
            emit error(-1, QString("Timeout waiting response for %1").arg(QString::fromUtf8(ba)));
            delete cmd;
            continue;
        }
        QByteArray resp = _socket->readAll();
        qDebug() << "sendCommandImpl: received" << resp.size() << "bytes for" << ba.trimmed();
        emit dataFromVNA(QString::fromUtf8(resp), cmd);
    }
}

void Socket::startScan(const QString& ip, quint16 port, int startKHz, int stopKHz, int points, int band, double powerDbM, int powerFreqKHz)
{
    if (QThread::currentThread() != _thread) {
        QMetaObject::invokeMethod(this, "startScan", Qt::QueuedConnection,
                                  Q_ARG(QString, ip),
                                  Q_ARG(quint16, port),
                                  Q_ARG(int, startKHz),
                                  Q_ARG(int, stopKHz),
                                  Q_ARG(int, points),
                                  Q_ARG(int, band),
                                  Q_ARG(double, powerDbM),
                                  Q_ARG(int, powerFreqKHz));
        return;
    }

    qDebug() << "startScan in socket thread, ip:" << ip << "port:" << port
             << "power:" << powerDbM << "dBm, power freq:" << powerFreqKHz << "kHz";

    QHostAddress hostAddr;
    if (!hostAddr.setAddress(ip)) {
        emit error(-1, QString("Invalid IP: %1").arg(ip));
        return;
    }

    _host = hostAddr;
    _port = port;

    qint64 startHz = qint64(startKHz) * 1000LL;
    qint64 stopHz  = qint64(stopKHz)  * 1000LL;
    qint64 bwHz    = qint64(band);
    qint64 powerFreqHz = qint64(powerFreqKHz) * 1000LL;

    QVector<VNAcomand*> cmds;
    cmds.append(new SYSTEM_PRESET());
    cmds.append(new SOURCE_POWER_LEVEL(1, powerDbM));
    cmds.append(new SENS_FREQ_START(1, startHz));
    cmds.append(new SENS_FREQ_STOP(1, stopHz));
    cmds.append(new SENS_FREQ_FIXED(1, powerFreqHz));
    // cmds.append(new SENS_FREQ_CW(1, powerFreqHz));
    cmds.append(new SENS_SWE_POINT(1, points));
    cmds.append(new SENS_BWID(1, bwHz));
    cmds.append(new TRIGGER_SOURCE_BUS());
    cmds.append(new INITIATE_CONTINUOUS(1));

    sendCommandWithOPC(_host, _port, cmds);

    if (!_scanning) {
        _scanning = true;
        if (_fdatTimer) _fdatTimer->start();
    }

    qDebug() << "startScan configured with power" << powerDbM << "dBm and fixed frequency" << powerFreqKHz << "kHz";
}

void Socket::stopScan()
{
    if (QThread::currentThread() != _thread) {
        QMetaObject::invokeMethod(this, "stopScan", Qt::QueuedConnection);
        return;
    }
    qDebug() << "stopScan called";
    if (!_scanning) {
        qDebug() << "Already stopped";
        return;
    }
    _scanning = false;
    if (_fdatTimer && _fdatTimer->isActive()) {
        _fdatTimer->stop();
    }
    QVector<VNAcomand*> cmds;
    cmds.append(new ABORT_COMMAND());
    cmds.append(new INITIATE_SINGLE_SHOT(1));
    sendCommandImpl(_host, _port, cmds);
    qDebug() << "stopScan completed";
}

void Socket::requestFDAT()
{
    if (QThread::currentThread() != _thread) {
        QMetaObject::invokeMethod(this, "requestFDAT", Qt::QueuedConnection);
        return;
    }
    if (!_scanning || _activeTraceNumbers.isEmpty()) {
        return;
    }
    static bool busy = false;
    if (busy) {
        qDebug() << "requestFDAT: still busy, skip";
        return;
    }
    busy = true;
    qDebug() << "requestFDAT: begin";
    _socket->write("TRIGger:SEQuence:SINGle\n");
    _socket->flush();
    bool opcOk = waitForOperationsComplete(_opcTimeout);
    if (!opcOk) {
        qWarning() << "requestFDAT: OPC timeout/failed; continue attempt to read";
    }
    QVector<VNAcomand*> xCmds;
    xCmds.append(new CALC_TRACE_DATA_XAXIS(_activeTraceNumbers.first()));
    sendCommandImpl(_host, _port, xCmds);
    for (int tr : _activeTraceNumbers) {
        QVector<VNAcomand*> cmds;
        cmds.append(new CALC_TRACE_SELECT(tr));
        cmds.append(new CALC_TRACE_DATA_FDAT(tr));
        sendCommandImpl(_host, _port, cmds);
    }
    qDebug() << "requestFDAT: finished";
    busy = false;
}

void Socket::onConnected()
{
    qDebug() << "Socket: connected to" << _host.toString() << ":" << _port;
    emit connected();
}

void Socket::onDisconnected()
{
    qDebug() << "Socket: disconnected";
    _scanning = false;
    if (_fdatTimer && _fdatTimer->isActive()) _fdatTimer->stop();
    emit disconnected();
}

bool Socket::canConnect(const QString &ip, quint16 port)
{
    QTcpSocket probe;
    probe.connectToHost(ip, port);
    bool ok = probe.waitForConnected(3000);
    if (ok) {
        if (probe.state() != QAbstractSocket::UnconnectedState) {
            probe.disconnectFromHost();
            probe.waitForDisconnected(1000);
        }
    } else {
        qWarning() << "canConnect: failed to connect to" << ip << port << "err:" << probe.errorString();
    }
    return ok;
}

void Socket::setGraphSettings(int graphCount, const QVector<int>& traceNumbers)
{
    _currentGraphCount = graphCount;
    _activeTraceNumbers = traceNumbers;
    qDebug() << "Socket::setGraphSettings: graphCount =" << graphCount
             << ", traces =" << traceNumbers;
}
