#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QHostAddress>
#include "vnaclient.h"

class Socket : public VNAclient
{
    Q_OBJECT

public:
    explicit Socket(QObject* parent = nullptr);
    ~Socket() override;

    VNAclient* getInstance() override;

public slots:
    void startScan(int startKHz, int stopKHz, int points, int band) override;
    void stopScan() override;
    void sendCommand(const QHostAddress& host, quint16 port, const QVector<VNAcomand*>& commands) override;
    void setGraphSettings(int graphCount, const QVector<int>& traceNumbers) override;
    void requestFDAT() override;

    void startPowerMeasurement(int startKHz, int stopKHz, int points, int band);
    void stopPowerMeasurement();
    bool isPowerMeasuring() const { return _powerMeasuring; }

private slots:
    void onConnected();
    void onDisconnected();


private:
    QTcpSocket* _socket;
    QTimer* _fdatTimer;
    bool _scanning;
    bool _powerMeasuring;
    QHostAddress _host;
    quint16 _port;
    int _lastType;

    int _currentGraphCount;
    QVector<int> _activeTraceNumbers;

    int _powerStartKHz;
    int _powerStopKHz;
    int _powerPoints;
    int _powerBand;
};

#endif // SOCKET_H
