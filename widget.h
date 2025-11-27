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

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(VNAclient* client, QWidget* parent = nullptr);
    ~Widget();
    Q_INVOKABLE void startScanFromQml(const QString& ip, quint16 port, int startKHz, int stopKHz, int points, int band, double powerDbM);
    Q_INVOKABLE void stopScanFromQml(const QString& ip, int port);
    Q_INVOKABLE void applyGraphSettings(const QVariantList& graphs, const QVariantMap& params);
    Q_INVOKABLE void updateConnectionSettings(const QString& ip, quint16 port);
private slots:
    void dataFromVNA(const QString& data, VNAcomand* cmd);
    void errorMessage(int code, const QString& message);
private:
    void setupUi();
    void startSocketThread();
    void stopSocketThread();
    void setOptimalScanSettings();
    void showIpPortError(const QString &msg);
    void setupPowerMeasurement(int startKHz, int stopKHz, int points, int band);
    void cleanupPowerMeasurement();
    VNAclient* _vnaClient;
    CreaterChart* _chartManager;
    QChartView* _chartView;
    bool _powerMeasuringMode;
    QString _currentIP;
    QString _currentSweepType;
    quint16 _currentPort;
    int _currentStartKHz;
    int _currentStopKHz;
    int _currentPoints;
    int _currentBand;
    double _currentPowerDbM;
    QVector<qreal> _frequencyData;
};

#endif // WIDGET_H
