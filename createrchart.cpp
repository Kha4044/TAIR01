#include "createrchart.h"
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QDebug>
#include <limits>

CreaterChart::CreaterChart(QObject* parent)
    : QObject(parent)
    , _chart(nullptr)
    , _axisX(nullptr)
    , _axisY(nullptr)
{
    initializeChart();
}

CreaterChart::~CreaterChart()
{
    clearAllTraces();
}

void CreaterChart::initializeChart()
{
    if (_chart)
    {
        delete _chart;
    }

    _chart = new QChart();
    _chart->setTitle("VNA Data");
    _chart->setAnimationOptions(QChart::NoAnimation);
    _chart->legend()->setVisible(true);
    _chart->setTheme(QChart::ChartThemeDark);
}

void CreaterChart::setupAxes(const QString& xTitle, const QString& yTitle)
{
    if (!_axisX)
    {
        _axisX = new QValueAxis();
    }
    if (!_axisY)
    {
        _axisY = new QValueAxis();
    }

    _axisX->setTitleText(xTitle);
    _axisX->setLabelFormat("%.0f");
    _axisY->setTitleText(yTitle);
    _axisY->setLabelFormat("%.3f");

    _axisX->setGridLineVisible(true);
    _axisX->setMinorGridLineVisible(true);
    _axisX->setGridLineColor(QColor(80, 80, 80, 100));
    _axisX->setMinorGridLineColor(QColor(60, 60, 60, 50));

    _axisY->setGridLineVisible(true);
    _axisY->setMinorGridLineVisible(true);
    _axisY->setGridLineColor(QColor(80, 80, 80, 100));
    _axisY->setMinorGridLineColor(QColor(60, 60, 60, 50));

    if (_chart->axes().isEmpty())
    {
        _chart->addAxis(_axisX, Qt::AlignBottom);
        _chart->addAxis(_axisY, Qt::AlignLeft);
    }
}

void CreaterChart::addTrace(int traceNum, const QString& name, const QColor& color)
{
    if (_seriesMap.contains(traceNum))
    {
        return;
    }

    QLineSeries* series = new QLineSeries();
    series->setName(name);

    QPen pen(color);
    pen.setWidth(2);
    series->setPen(pen);

    _chart->addSeries(series);
    series->attachAxis(_axisX);
    series->attachAxis(_axisY);

    _seriesMap.insert(traceNum, series);
}

void CreaterChart::removeTrace(int traceNum)
{
    if (_seriesMap.contains(traceNum))
    {
        QLineSeries* series = _seriesMap.take(traceNum);
        _chart->removeSeries(series);
        delete series;
    }
}

void CreaterChart::clearAllTraces()
{
    for (auto it = _seriesMap.begin(); it != _seriesMap.end(); ++it)
    {
        _chart->removeSeries(it.value());
        delete it.value();
    }
    _seriesMap.clear();
}

void CreaterChart::updateTraceData(int traceNum, const QVector<qreal>& xData, const QVector<qreal>& yData)
{
    if (!_seriesMap.contains(traceNum))
    {
        return;
    }

    int minSize = qMin(xData.size(), yData.size());
    if (minSize == 0)
    {
        return;
    }

    QLineSeries* series = _seriesMap[traceNum];
    series->clear();

    for (int i = 0; i < minSize; ++i)
    {
        series->append(xData[i], yData[i]);
    }

    _chart->update();
}

void CreaterChart::autoScaleAxes()
{
    if (!_axisX || !_axisY || _seriesMap.isEmpty())
    {
        return;
    }

    qreal xMin = std::numeric_limits<qreal>::max();
    qreal xMax = std::numeric_limits<qreal>::lowest();
    qreal yMin = std::numeric_limits<qreal>::max();
    qreal yMax = std::numeric_limits<qreal>::lowest();

    bool hasData = false;

    for (QLineSeries* series : _seriesMap)
    {
        auto points = series->points();
        if (points.isEmpty()) continue;

        hasData = true;

        for (const QPointF& point : points)
        {
            xMin = qMin(xMin, point.x());
            xMax = qMax(xMax, point.x());
            yMin = qMin(yMin, point.y());
            yMax = qMax(yMax, point.y());
        }
    }

    if (hasData)
    {
        _axisX->setRange(xMin, xMax);
        _axisY->setRange(yMin, yMax);
    }
}
