#ifndef SOCKET_H
#define SOCKET_H

#include "vnaclient.h"
#include "vnacomand.h"
#include <QTcpSocket>
#include <QTimer>
#include <QThread>
#include <QVector>
#include <QHostAddress>

class Socket : public VNAclient
{
    Q_OBJECT

public:
    explicit Socket(QObject* parent = nullptr);
    ~Socket();

    VNAclient* getInstance() override;
    void setTimeouts(int normalTimeoutMs, int opcTimeoutMs, int fdatIntervalMs);
    void startThread();
    void stopThread();
    void setGraphSettings(int graphCount, const QVector<int>& traceNumbers) override;
    bool canConnect(const QString &ip, quint16 port);

public slots:
    void sendCommand(const QHostAddress& host, quint16 port, const QVector<VNAcomand*>& commands) override;
    void startScan(const QString& ip, quint16 port, int startKHz, int stopKHz, int points, int band, double powerDbM, int powerFreqKHz) override;
    void stopScan() override;

private slots:
    void initializeInThread();
    void cleanupInThread();
    void stopInThread();
    void onConnected();
    void onDisconnected();
    void requestFDAT();

private:
    void sendCommandImpl(const QHostAddress& host, quint16 port, const QVector<VNAcomand*>& commands);
    bool ensureConnection(const QHostAddress& host, quint16 port);
    bool waitForOperationsComplete(int timeoutMs);
    void sendCommandWithOPC(const QHostAddress& host, quint16 port, const QVector<VNAcomand*>& commands);

    QTcpSocket* _socket;
    QTimer* _fdatTimer;
    QThread* _thread;

    bool _scanning;
    int _currentGraphCount;
    QVector<int> _activeTraceNumbers;

    int _normalTimeout;
    int _opcTimeout;
    int _fdatInterval;

    QHostAddress _host;
    quint16 _port;
};

#endif // SOCKET_H
