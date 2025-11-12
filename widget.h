#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMap>
#include <QTimer>
#include <QChartView>
#include "vnaclient.h"
#include "createrchart.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(VNAclient* vnaClient, QWidget* parent = nullptr);
    ~Widget();

    Q_INVOKABLE void startScanFromQml(int startKHz, int stopKHz, int points, int band);
    Q_INVOKABLE void stopScanFromQml();
    Q_INVOKABLE void applyGraphSettings(const QVariantList& graphs, const QVariantMap& params);
    Q_INVOKABLE void setPowerMeasuringMode(bool enabled);
    Q_INVOKABLE void forceDataSync();

signals:
    void sendCommandToVNA(QHostAddress host, quint16 port, QVector<VNAcomand*> commands);

private slots:
    void dataFromVNA(const QString& data, VNAcomand* cmd);
    void errorMessage(int code, const QString& message);

private:
    VNAclient* _vnaClient;
    CreaterChart* _chartManager;
    QChartView* _chartView;
    QVector<qreal> _frequencyData;

    bool _powerMeasuringMode;
    QVector<qreal> _powerFrequencyData;
    QVector<qreal> _powerValueData;

    void setupUi();
    void requestFrequencyData();
    void addTestData();
    void setupPowerMeasurement(int startKHz, int stopKHz, int points, int band);
    void cleanupPowerMeasurement();
};

#endif // WIDGET_H
