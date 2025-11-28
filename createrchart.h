#ifndef CREATERCHART_H
#define CREATERCHART_H

#include <QObject>
#include <QMap>
#include <QColor>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>

class CreaterChart : public QObject
{
    Q_OBJECT

public:
    explicit CreaterChart(QObject* parent = nullptr);
    ~CreaterChart();

    QChart* getChart() { return _chart; }

    void setupAxes(const QString& xTitle = "Frequency (kHz)",
                   const QString& yTitle = "Amplitude");
    void addTrace(int traceNum, const QString& name, const QColor& color);
    void removeTrace(int traceNum);
    void clearAllTraces();

    void updateTraceData(int traceNum, const QVector<qreal>& xData, const QVector<qreal>& yData);
    void autoScaleAxes();

    bool hasTrace(int traceNum) const { return _seriesMap.contains(traceNum); }

private:
    QChart* _chart;
    QValueAxis* _axisX;
    QValueAxis* _axisY;
    QMap<int, QLineSeries*> _seriesMap;
};

#endif // CREATERCHART_H
