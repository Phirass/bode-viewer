#include "BodePlotPanel.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QChart>
#include <QChartView>
#include <QLegend>
#include <QLineSeries>
#include <QLogValueAxis>
#include <QValueAxis>
#include <QPen>
#include <QColor>

#include <algorithm>
#include <cmath>

// Ab Qt 6 liegen die Qt-Charts-Klassen im globalen Namensraum; das folgende
// Makro ist dort leer und schadet nicht. Unter Qt 5 loest es "QtCharts::" auf.
#ifdef QT_CHARTS_USE_NAMESPACE
QT_CHARTS_USE_NAMESPACE
#endif

BodePlotPanel::BodePlotPanel(QWidget *parent)
    : QWidget(parent)
{
    m_magView = new QChartView(this);
    m_phaseView = new QChartView(this);
    m_magView->setRenderHint(QPainter::Antialiasing);
    m_phaseView->setRenderHint(QPainter::Antialiasing);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_magView);
    layout->addWidget(m_phaseView);
    setLayout(layout);
}

void BodePlotPanel::setData(const BodeData &data, const StabilityResult &stability)
{
    buildMagnitudeChart(data, stability);
    buildPhaseChart(data, stability);
}

void BodePlotPanel::buildMagnitudeChart(const BodeData &data,
                                        const StabilityResult &stability)
{
    auto *chart = new QChart();
    chart->setTitle(QStringLiteral("Amplitudengang"));
    chart->legend()->hide();

    // Kurve des Betragsverlaufs.
    auto *curve = new QLineSeries();
    curve->setName(QStringLiteral("|G(jw)|"));
    double yMin = 1e300, yMax = -1e300;
    for (std::size_t i = 0; i < data.size(); ++i) {
        curve->append(data.omega[i], data.magnitudeDb[i]);
        yMin = std::min(yMin, data.magnitudeDb[i]);
        yMax = std::max(yMax, data.magnitudeDb[i]);
    }
    chart->addSeries(curve);

    if (data.isEmpty()) { yMin = -60; yMax = 20; }
    // Etwas Rand oben/unten, damit die Kurve nicht am Rahmen klebt.
    const double pad = std::max(5.0, (yMax - yMin) * 0.1);
    yMin -= pad;
    yMax += pad;

    // Logarithmische Frequenzachse.
    auto *axisX = new QLogValueAxis();
    axisX->setTitleText(QStringLiteral("Frequenz w [rad/s]"));
    axisX->setBase(10.0);
    axisX->setMinorTickCount(-1);
    if (!data.isEmpty())
        axisX->setRange(data.omega.front(), data.omega.back());
    chart->addAxis(axisX, Qt::AlignBottom);
    curve->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setTitleText(QStringLiteral("Betrag [dB]"));
    axisY->setRange(yMin, yMax);
    chart->addAxis(axisY, Qt::AlignLeft);
    curve->attachAxis(axisY);

    // Referenzlinie 0 dB (grau, gestrichelt).
    if (!data.isEmpty()) {
        auto *zeroDb = new QLineSeries();
        zeroDb->append(data.omega.front(), 0.0);
        zeroDb->append(data.omega.back(), 0.0);
        QPen p(QColor(120, 120, 120));
        p.setStyle(Qt::DashLine);
        zeroDb->setPen(p);
        chart->addSeries(zeroDb);
        zeroDb->attachAxis(axisX);
        zeroDb->attachAxis(axisY);
    }

    // Senkrechte Markierung an der Amplitudendurchtrittsfrequenz.
    if (stability.hasPhaseMargin) {
        auto *marker = new QLineSeries();
        marker->append(stability.gainCrossoverW, yMin);
        marker->append(stability.gainCrossoverW, yMax);
        QPen p(QColor(200, 40, 40));
        p.setStyle(Qt::DotLine);
        marker->setPen(p);
        chart->addSeries(marker);
        marker->attachAxis(axisX);
        marker->attachAxis(axisY);
    }

    // Alten Chart der Ansicht freigeben und neuen setzen.
    QChart *old = m_magView->chart();
    m_magView->setChart(chart);
    delete old;
}

void BodePlotPanel::buildPhaseChart(const BodeData &data,
                                    const StabilityResult &stability)
{
    auto *chart = new QChart();
    chart->setTitle(QStringLiteral("Phasengang"));
    chart->legend()->hide();

    auto *curve = new QLineSeries();
    curve->setName(QStringLiteral("arg G(jw)"));
    double yMin = 1e300, yMax = -1e300;
    for (std::size_t i = 0; i < data.size(); ++i) {
        curve->append(data.omega[i], data.phaseDeg[i]);
        yMin = std::min(yMin, data.phaseDeg[i]);
        yMax = std::max(yMax, data.phaseDeg[i]);
    }
    chart->addSeries(curve);

    if (data.isEmpty()) { yMin = -270; yMax = 0; }
    // -180 Grad soll immer sichtbar sein (fuer die Ablesung des Amplitudenrands).
    yMin = std::min(yMin, -180.0);
    yMax = std::max(yMax, 0.0);
    const double pad = std::max(5.0, (yMax - yMin) * 0.1);
    yMin -= pad;
    yMax += pad;

    auto *axisX = new QLogValueAxis();
    axisX->setTitleText(QStringLiteral("Frequenz w [rad/s]"));
    axisX->setBase(10.0);
    axisX->setMinorTickCount(-1);
    if (!data.isEmpty())
        axisX->setRange(data.omega.front(), data.omega.back());
    chart->addAxis(axisX, Qt::AlignBottom);
    curve->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setTitleText(QStringLiteral("Phase [Grad]"));
    axisY->setRange(yMin, yMax);
    chart->addAxis(axisY, Qt::AlignLeft);
    curve->attachAxis(axisY);

    // Referenzlinie -180 Grad.
    if (!data.isEmpty()) {
        auto *ref = new QLineSeries();
        ref->append(data.omega.front(), -180.0);
        ref->append(data.omega.back(), -180.0);
        QPen p(QColor(120, 120, 120));
        p.setStyle(Qt::DashLine);
        ref->setPen(p);
        chart->addSeries(ref);
        ref->attachAxis(axisX);
        ref->attachAxis(axisY);
    }

    // Senkrechte Markierung an der Phasendurchtrittsfrequenz.
    if (stability.hasGainMargin) {
        auto *marker = new QLineSeries();
        marker->append(stability.phaseCrossoverW, yMin);
        marker->append(stability.phaseCrossoverW, yMax);
        QPen p(QColor(200, 40, 40));
        p.setStyle(Qt::DotLine);
        marker->setPen(p);
        chart->addSeries(marker);
        marker->attachAxis(axisX);
        marker->attachAxis(axisY);
    }

    QChart *old = m_phaseView->chart();
    m_phaseView->setChart(chart);
    delete old;
}
