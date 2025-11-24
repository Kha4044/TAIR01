#ifndef VNACLIENT_H
#define VNACLIENT_H

#include <QObject>
#include <QHostAddress>
#include <QVector>
#include <QString>
#include "vnacomand.h"

class VNAclient : public QObject {
    Q_OBJECT
public:
    explicit VNAclient(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~VNAclient() = default;

    virtual VNAclient* getInstance() = 0;



public slots:

    virtual void startScan(const QString& ip, quint16 port, int startKHz, int stopKHz, int points, int band) = 0;
    virtual void stopScan() = 0;
    virtual void sendCommand(const QHostAddress &host, quint16 port, const QVector<VNAcomand*> &commands) = 0;
    virtual void setGraphSettings(int graphCount, const QVector<int>& traceNumbers) = 0;
    virtual void requestFDAT() = 0;



signals:
    void connected();
    void disconnected();
    void error(int errorCode, const QString &message);
    void dataFromVNA(const QString &data, VNAcomand *cmd);
};

#endif // VNACLIENT_H
