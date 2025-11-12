#ifndef VNACONTROLLER_H
#define VNACONTROLLER_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QHostAddress>
#include "vnacomand.h"
#include "vnaclient.h"

// Информация о трейсе (графике) хранится здесь
struct TraceInfo {
    int num;            // 1..16
    QString param;      // "S11" / "S21" / ...
    QString unit;       // отображаемая строка (для UI). На приборе — формат команды CALC:FORM? (можно мапить)
};

class VnaController : public QObject {
    Q_OBJECT
public:
    explicit VnaController(VNAclient *client, QObject *parent = nullptr);
    ~VnaController() override;

    // Настроечные методы (вызываются из Widget/QML)
    Q_SLOT void configureAndStart(int startKHz, int stopKHz, int points, int bandwidthHz,
                                  const QVector<TraceInfo> &traces);
    Q_SLOT void updateTraces(const QVector<TraceInfo> &traces);
    Q_SLOT void stop();

signals:
    // отправить набор команд на клиент (Widget подписан и пробросит на Socket в отдельном потоке)
    void sendCommands(const QHostAddress &host, quint16 port, QVector<VNAcomand*> commands);

    // сигнал для UI: пришли данные по трейсу
    void traceDataReady(int traceNum, QVector<qreal> values);

    // если контроллер получил от клиента сырый ответ и хочет его показать
    void rawVnaResponse(const QString &scpiCmd, const QString &response);

private slots:
    void onClientDataReceived(const QString &data, VNAcomand *cmd);

private:
    VNAclient *m_client;
    QTimer *m_pollTimer = nullptr;
    QVector<TraceInfo> m_traces;
    int m_currentPollIndex = 0;

    // параметры сканирования
    qint64 m_startHz = 0;
    qint64 m_stopHz = 0;
    int m_points = 201;
    qint64 m_bandHz = 10000;

    void configureInstrument();      // формирует и посылает команды конфигурации
    void pollNextTrace();            // посылает CALC:SEL:DATA:FDAT? для следующего трейса
};

#endif // VNACONTROLLER_H
