#ifndef WIDGET_H
#define WIDGET_H

#include "vnaclient.h"
#include "createrchart.h"
#include <QWidget>
#include <QChartView>
#include <QVector>
#include <QHash>
#include <QColor>

class VNAclient;
class CreaterChart;
class Socket;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(VNAclient* client, QWidget* parent = nullptr);
    ~Widget();

    // QML доступные методы
    Q_INVOKABLE void startScanFromQml(const QString& ip, quint16 port, int startKHz, int stopKHz, int points, int band);
    Q_INVOKABLE void stopScanFromQml(const QString& ip, int port);
    Q_INVOKABLE void applyGraphSettings(const QVariantList& graphs, const QVariantMap& params);
    Q_INVOKABLE void setPowerMeasuringMode(bool enabled);
    Q_INVOKABLE void updateConnectionSettings(const QString& ip, quint16 port);

private slots:
    void dataFromVNA(const QString& data, VNAcomand* cmd);
    void errorMessage(int code, const QString& message);
    void requestFrequencyData();
    void forceDataSync();

private:
    void setupUi();
    void startSocketThread();
    void stopSocketThread();
    void setOptimalScanSettings();
    void addTestData();
    void showIpPortError(const QString &msg);
    void setupPowerMeasurement(int startKHz, int stopKHz, int points, int band);
    void cleanupPowerMeasurement();

    VNAclient* _vnaClient;
    CreaterChart* _chartManager;
    QChartView* _chartView;

    bool _powerMeasuringMode;
    QString _currentIP;
    quint16 _currentPort;
    int _currentStartKHz;
    int _currentStopKHz;
    int _currentPoints;
    int _currentBand;

    QVector<qreal> _frequencyData;
    QVector<qreal> _powerFrequencyData;
    QVector<qreal> _powerValueData;
};

#endif // WIDGET_H
