#include "vnacontroller.h"
#include <QDebug>

#define VNA_HOST QHostAddress::LocalHost
#define VNA_PORT 5025

VnaController::VnaController(VNAclient *client, QObject *parent)
    : QObject(parent),
    m_client(client),
    m_pollTimer(new QTimer(this)),
    m_currentPollIndex(0),
    m_startHz(0),
    m_stopHz(0),
    m_points(201),
    m_bandHz(10000)
{
    connect(m_client, &VNAclient::dataFromVNA, this, &VnaController::onClientDataReceived);
    m_pollTimer->setInterval(300);
    connect(m_pollTimer, &QTimer::timeout, this, &VnaController::pollNextTrace);
}

VnaController::~VnaController()
{
    stop();
}

void VnaController::configureAndStart(int startKHz, int stopKHz, int points, int bandwidthHz,
                                      const QVector<TraceInfo> &traces)
{
    m_startHz = qint64(startKHz) * 1000LL;
    m_stopHz  = qint64(stopKHz) * 1000LL;
    m_points  = points;
    m_bandHz  = qint64(bandwidthHz);

    m_traces = traces;

    configureInstrument();

    m_currentPollIndex = 0;
    if (!m_traces.isEmpty()) m_pollTimer->start();
}

void VnaController::updateTraces(const QVector<TraceInfo> &traces)
{
    m_traces = traces;
    configureInstrument();
}

void VnaController::stop()
{
    if (m_pollTimer->isActive()) m_pollTimer->stop();
    QVector<VNAcomand*> *cmds = new QVector<VNAcomand*>();
    cmds->append(new VNAcomand(false, 0, ":ABOR\n"));
    emit sendCommands(VNA_HOST, VNA_PORT, cmds);
}

static QString unitToFormatToken(const QString &unit)
{
    // Простая мэппинг-таблица: расширь при необходимости.
    // Ожидаемые возвращаемые форматы — те, которые прибор понимает (MLOG, PHASe, GDELay, SLIN, SLOG, SCOMplex, SMITh, SADMittance, PLINear, PLOGarithmic, POLar, MLINear, SWR, REAL, IMAGinary, UPHase)
    QString u = unit.trimmed().toLower();
    if (u.contains("амп") && u.contains("лог")) return "MLOG";
    if (u.contains("амп") && u.contains("лин")) return "MLIN";
    if (u.contains("фаза") && u.contains("180")) return "UPHASE";
    if (u.contains("фаза")) return "PHASe";
    if (u.contains("гвз") || u.contains("gdel")) return "GDELay";
    if (u.contains("реал")) return "REAL";
    if (u.contains("мним")) return "IMAGinary";
    if (u.contains("вольп") && u.contains("лог")) return "SLOG";
    if (u.contains("вольп") && u.contains("лин")) return "SLIN";
    if (u.contains("smith") || u.contains("волп (r+jx)") || u.contains("r+j")) return "SMITh";
    if (u.contains("scomplex") || u.contains("real/imag") || u.contains("re/im")) return "SCOMplex";
    if (u.contains("polar") || u.contains("поляр")) return "POLar";
    if (u.contains("swr")) return "SWR";
    // по умолчанию — логарифм амплитуды
    return "MLOG";
}

void VnaController::configureInstrument()
{
    // Формируем команды и отправляем единым пакетом.
    QVector<VNAcomand*> *cmds = new QVector<VNAcomand*>();

    // 0) Системная инициализация: перезагрузка / preset
    cmds->append(new SYSTEM_PRESET());

    // 1) Выбрать активный канал 1
    cmds->append(new SYSTEM_CHANNEL_SELECT(1));

    // 2) Активировать окно дисплея 1 (если нужно)
    cmds->append(new DISPLAY_WINDOW_ACTIVATE(1));

    // 3) Назначаем источник триггера программный (BUS)
    cmds->append(new TRIGGER_SEQUENCE_SOURCE("BUS"));

    // 4) Настроить режим непрерывной инициации для канала 1 (вкл/выкл — решение за UI)
    cmds->append(new INIT_CONT_MODE(1, true));

    // 5) Сброс/установка частот/точек/BW
    cmds->append(new SENS_FREQ_START(1, m_startHz));
    cmds->append(new SENS_FREQ_STOP(1, m_stopHz));
    cmds->append(new SENS_SWE_POINT(1, m_points));
    cmds->append(new SENS_BWID(1, m_bandHz));

    // 6) Установка количества параметров/трейсов в калькуляторе в соответствии с количеством трасс
    int paramCount = qMax(1, m_traces.size());
    cmds->append(new CALC_PARAMETER_COUNT(paramCount, 1));

    // 7) Очистим / удалим трейсы, которых нет, и создадим новые согласно m_traces
    const int maxTr = 16;
    for (int t = 1; t <= maxTr; ++t) {
        bool found = false;
        for (const TraceInfo &tr : m_traces) if (tr.num == t) { found = true; break; }
        if (!found) {
            cmds->append(new VNAcomand(false, 0, QString("CALCulate1:PARameter:DEL 'Tr%1'\n").arg(t)));
        }
    }

    for (const TraceInfo &tr : m_traces) {
        // Определяем параметр (S11...): CALCulate1:PARameterN:DEF <param>
        cmds->append(new CALC_PARAMETER_DEFINE(1, tr.num, tr.param));

        // Установим формат отображения для трассы: CALCulate1:TRACeN:FORMat <fmt>
        QString fmt = unitToFormatToken(tr.unit);
        cmds->append(new CALC_TRACE_FORMAT(1, tr.num, fmt));
    }

    // Отправляем пакет команд
    emit sendCommands(VNA_HOST, VNA_PORT, cmds);
    // Socket получит владение cmds и удалит его после выполнения.
}
