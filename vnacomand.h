#ifndef VNACOMAND_H
#define VNACOMAND_H

#include <QString>
#include <QVector>

class VNAcomand
{
public:
    bool request;
    int type;
    QString SCPI;

    VNAcomand(bool request_ = false, int type_ = 0, const QString& scpi = QString())
        : request(request_), type(type_), SCPI(scpi) {}
    virtual ~VNAcomand() = default;
};

class VNAcomand_REAL : public VNAcomand
{
public:
    VNAcomand_REAL(bool request_, int type_, const QString& scpi = QString())
        : VNAcomand(request_, type_, scpi) {}
    virtual ~VNAcomand_REAL() = default;
    virtual QVector<qreal> parseResponse(const QString& data) const = 0;
};

class CALC_DATA_FDAT : public VNAcomand_REAL
{
public:
    CALC_DATA_FDAT(int type = 1) : VNAcomand_REAL(true, type, "CALC:DATA:FDAT?\n") {}
    QVector<qreal> parseResponse(const QString& data) const override;
};

class CALC_TRACE_DATA_FDAT : public VNAcomand_REAL
{
public:
    CALC_TRACE_DATA_FDAT(int traceNum)
        : VNAcomand_REAL(true, traceNum, QString("CALC:TRAC%1:DATA:FDAT?\n").arg(traceNum)) {}
    QVector<qreal> parseResponse(const QString& data) const override;
};

class SENS_FREQ_START : public VNAcomand_REAL
{
public:
    SENS_FREQ_START(int type, qint64 freqHz)
        : VNAcomand_REAL(false, type, QString("SENS:FREQ:STAR %1\n").arg(freqHz)) {}
    QVector<qreal> parseResponse(const QString& data) const override { Q_UNUSED(data); return {}; }
};

class SENS_FREQ_STOP : public VNAcomand_REAL
{
public:
    SENS_FREQ_STOP(int type, qint64 freqHz)
        : VNAcomand_REAL(false, type, QString("SENS:FREQ:STOP %1\n").arg(freqHz)) {}
    QVector<qreal> parseResponse(const QString& data) const override { Q_UNUSED(data); return {}; }
};

class SENS_SWE_POINT : public VNAcomand_REAL
{
public:
    SENS_SWE_POINT(int type, int points)
        : VNAcomand_REAL(false, type, QString("SENS:SWE:POIN %1\n").arg(points)) {}
    QVector<qreal> parseResponse(const QString& data) const override { Q_UNUSED(data); return {}; }
};

class SENS_BWID : public VNAcomand_REAL
{
public:
    SENS_BWID(int type, qint64 bwHz)
        : VNAcomand_REAL(false, type, QString("SENS:BAND %1\n").arg(bwHz)) {}
    QVector<qreal> parseResponse(const QString& data) const override { Q_UNUSED(data); return {}; }
};

class SYSTEM_PRESET : public VNAcomand
{
public:
    SYSTEM_PRESET() : VNAcomand(false, 0, "SYST:PRESet\n") {}
};

class SYST_CHAN : public VNAcomand
{
public:
    explicit SYST_CHAN(int ch) : VNAcomand(false, ch, QString("SYST:CHAN %1\n").arg(ch)) {}
};

class DISPLAY_WINDOW_ACTIVATE : public VNAcomand
{
public:
    explicit DISPLAY_WINDOW_ACTIVATE(int window = 1)
        : VNAcomand(false, window, QString("DISP:WINDow%1:ACTivate()\n").arg(window)) {}
};

class TRIGGER_SOURCE : public VNAcomand
{
public:
    explicit TRIGGER_SOURCE(const QString& src = "BUS")
        : VNAcomand(false, 0, QString("TRIGger:SEQuence:SOURce %1\n").arg(src)) {}
};

class INIT_CONT_MODE : public VNAcomand
{
public:
    explicit INIT_CONT_MODE(int ch = 1, bool on = true)
        : VNAcomand(false, ch, QString("INITiate%1:CONTinuous %2\n").arg(ch).arg(on ? "ON" : "OFF")) {}
};

class CALC_PARAMETER_COUNT : public VNAcomand_REAL
{
public:
    CALC_PARAMETER_COUNT(int count)
        : VNAcomand_REAL(false, 0, QString("CALC1:PAR:COUN %1\n").arg(count)) {}
    QVector<qreal> parseResponse(const QString&) const override { return {}; }
};

class CALC_PARAMETER_DEFINE : public VNAcomand_REAL
{
public:
    CALC_PARAMETER_DEFINE(int traceNum, const QString& param)
        : VNAcomand_REAL(false, 0, QString("CALC1:PAR%1:DEF %2\n").arg(traceNum).arg(param)) {}
    QVector<qreal> parseResponse(const QString&) const override { return {}; }
};

class CALC_TRACE_FORMAT : public VNAcomand_REAL
{
public:
    CALC_TRACE_FORMAT(int traceNum, const QString& format)
        : VNAcomand_REAL(false, 0, QString("CALC1:TRAC%1:FORM %2\n").arg(traceNum).arg(format)) {}
    QVector<qreal> parseResponse(const QString&) const override { return {}; }
};

class CALC_TRACE_SELECT : public VNAcomand_REAL
{
public:
    CALC_TRACE_SELECT(int traceNum)
        : VNAcomand_REAL(false, 0, QString("CALC1:PAR%1:SEL\n").arg(traceNum)) {}
    QVector<qreal> parseResponse(const QString&) const override { return {}; }
};

class DISP_WIND_TRACE : public VNAcomand_REAL
{
public:
    DISP_WIND_TRACE(int windowNum, int traceNum)
        : VNAcomand_REAL(false, 0, QString("DISP:WIND%1:TRAC%2:ACT\n").arg(windowNum).arg(traceNum)) {}
    QVector<qreal> parseResponse(const QString&) const override { return {}; }
};

class CALC_TRACE_DATA_XAXIS : public VNAcomand_REAL
{
public:
    CALC_TRACE_DATA_XAXIS(int traceNum)
        : VNAcomand_REAL(true, traceNum, QString("CALC:TRAC%1:DATA:XAXIS?\n").arg(traceNum)) {}
    QVector<qreal> parseResponse(const QString& data) const override;
};

class SOURCE_POWER_COUPLE : public VNAcomand
{
public:
    SOURCE_POWER_COUPLE(int channel, bool couple)
        : VNAcomand(false, 0, QString("SOURce%1:POWer:PORT:COUPle %2\n").arg(channel).arg(couple ? "ON" : "OFF")) {}
};

class SOURCE_POWER_LEVEL_SET : public VNAcomand
{
public:
    SOURCE_POWER_LEVEL_SET(int channel, qreal powerDbM)
        : VNAcomand(false, 0, QString("SOURce%1:POWer:LEVel:IMMediate:AMPLitude %2\n").arg(channel).arg(powerDbM)) {}
};

class SOURCE_POWER_LEVEL_GET : public VNAcomand_REAL
{
public:
    SOURCE_POWER_LEVEL_GET(int channel = 1)
        : VNAcomand_REAL(true, 100 + channel, QString("SOURce%1:POWer:LEVel:IMMediate:AMPLitude?\n").arg(channel)) {}
    QVector<qreal> parseResponse(const QString& data) const override;
};

class OUTPUT_PORT_STATE : public VNAcomand
{
public:
    OUTPUT_PORT_STATE(int port, bool state)
        : VNAcomand(false, 0, QString("OUTPut%1:STATe %2\n").arg(port).arg(state ? "ON" : "OFF")) {}
};

class CALC_TRACE_FORMAT_POWER : public VNAcomand
{
public:
    CALC_TRACE_FORMAT_POWER(int traceNum, const QString& format)
        : VNAcomand(false, 0, QString("CALC:TRAC%1:FORM %2\n").arg(traceNum).arg(format)) {}
};

class CALC_PARAMETER_POWER : public VNAcomand
{
public:
    CALC_PARAMETER_POWER(int traceNum, const QString& parameter)
        : VNAcomand(false, 0, QString("CALC:PAR%1:DEF %2\n").arg(traceNum).arg(parameter)) {}
};

class CALC_TRACE_DATA_POWER : public VNAcomand_REAL
{
public:
    CALC_TRACE_DATA_POWER(int traceNum)
        : VNAcomand_REAL(true, traceNum, QString("CALC:TRAC%1:DATA:FDAT?\n").arg(traceNum)) {}
    QVector<qreal> parseResponse(const QString& data) const override;
};

class TRIGGER_SINGL : public VNAcomand
{
public:
    TRIGGER_SINGL()
        : VNAcomand(false, 0, "TRIGger:SEQuence:SINGL\n") {}
};

class OPC_QUERY : public VNAcomand
{
public:
    OPC_QUERY() : VNAcomand(true, 0, "*OPC?\n") {}
};

class WAIT_FOR_OPC : public VNAcomand
{
public:
    WAIT_FOR_OPC() : VNAcomand(false, 0, "*WAI\n") {}
};

class ABORT_COMMAND : public VNAcomand
{
public:
    ABORT_COMMAND() : VNAcomand(false, 0, ":ABOR\n") {}
};

#endif // VNACOMAND_H
