#include "vnacomand.h"
#include <QStringList>
#include <QDebug>

static QVector<qreal> parseCsvToReals(const QString& data)
{
    QVector<qreal> out;
    QStringList parts = data.trimmed().split(',', Qt::SkipEmptyParts);
    out.reserve(parts.size());
    for (const QString& p : parts)
    {
        bool ok = false;
        double v = p.toDouble(&ok);
        if (ok) out.append((qreal)v);
    }
    return out;
}

QVector<qreal> CALC_DATA_FDAT::parseResponse(const QString& data) const
{
    QVector<qreal> all = parseCsvToReals(data);
    QVector<qreal> out;
    out.reserve((all.size()+1)/2);
    for (int i = 0; i < all.size(); i += 2)
        out.append(all[i]);
    return out;
}

QVector<qreal> CALC_TRACE_DATA_FDAT::parseResponse(const QString& data) const
{
    QVector<qreal> all = parseCsvToReals(data);
    QVector<qreal> out;
    out.reserve((all.size()+1)/2);
    for (int i = 0; i < all.size(); i += 2)
        out.append(all[i]);
    return out;
}

QVector<qreal> CALC_TRACE_DATA_XAXIS::parseResponse(const QString& data) const
{
    QVector<qreal> values = parseCsvToReals(data);

    for (int i = 0; i < values.size(); ++i)
    {
        values[i] = values[i] / 1000.0;
    }

    return values;
}

QVector<qreal> SOURCE_POWER_LEVEL_GET::parseResponse(const QString& data) const
{
    QVector<qreal> out;
    bool ok = false;
    double powerValue = data.trimmed().toDouble(&ok);

    if (ok)
    {
        out.append(powerValue);
    }
    else
    {
        out.append(0.0);
    }

    return out;
}

QVector<qreal> CALC_TRACE_DATA_POWER::parseResponse(const QString& data) const
{
    QVector<qreal> all = parseCsvToReals(data);
    QVector<qreal> out;
    out.reserve((all.size()+1)/2);

    for (int i = 0; i < all.size(); i += 2)
    {
        out.append(all[i]);
    }

    return out;
}
