#include "vnaclient.h"
#include <QTcpSocket>
#include <QDebug>

VNAclient::VNAclient(QObject *parent) : QObject(parent) {}

void VNAclient::sendCommandToVNA(const QHostAddress &addr, int port, const QVector<VNAcomand*> &commands)
{
    QTcpSocket socket;
    socket.connectToHost(addr, port);
    if (!socket.waitForConnected(1000)) {
        qWarning() << "Не удалось подключиться";
        return;
    }

    for (auto *cmd : commands) {
        socket.write(cmd->SCPI.toUtf8());
        socket.flush();
        socket.waitForBytesWritten(100);
    }

    socket.disconnectFromHost();
}

void VNAclient::startScan(int startKHz, int stopKHz, int points, int band)
{
    QVector<VNAcomand*> cmds;
    cmds.append(new SENS_FREQ_START(1, startKHz * 1000.0));
    cmds.append(new SENS_FREQ_STOP(1, stopKHz * 1000.0));
    cmds.append(new SENS_SWE_POINT(1, points));
    cmds.append(new SENS_BWID(1, band));

    sendCommandToVNA(QHostAddress::LocalHost, 5025, cmds);
    qDeleteAll(cmds);
}

void VNAclient::stopScan()
{
    QVector<VNAcomand*> cmds;
    auto *cmd = new VNAcomand(false, 1);
    cmd->SCPI = ":ABOR\n";
    cmds.append(cmd);
    sendCommandToVNA(QHostAddress::LocalHost, 5025, cmds);
    qDeleteAll(cmds);
}
