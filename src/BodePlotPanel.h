#ifndef BODEPLOTPANEL_H
#define BODEPLOTPANEL_H

#include <QWidget>
#include "BodeCalculator.h"
#include "StabilityAnalyzer.h"

QT_BEGIN_NAMESPACE
class QChart;
class QChartView;
QT_END_NAMESPACE

/**
 * @brief Plot-Modul (auf Basis von Qt Charts): stellt das Bode-Diagramm dar.
 *
 * Das Panel enthaelt zwei uebereinander angeordnete Diagramme:
 *   - oben:  Amplitudengang (Betrag in dB) ueber logarithmischer Frequenzachse
 *   - unten: Phasengang (Phase in Grad) ueber logarithmischer Frequenzachse
 *
 * Zusaetzlich werden die Amplituden- und Phasendurchtrittsfrequenzen als
 * senkrechte Markierungen sowie die Referenzlinien 0 dB und -180 Grad
 * eingezeichnet. Das Panel kennt nur Daten (BodeData/StabilityResult) und
 * ist von Berechnung und GUI entkoppelt.
 */
class BodePlotPanel : public QWidget
{
    Q_OBJECT
public:
    explicit BodePlotPanel(QWidget *parent = nullptr);

    /** @brief Aktualisiert beide Diagramme mit neuen Daten. */
    void setData(const BodeData &data, const StabilityResult &stability);

private:
    void buildMagnitudeChart(const BodeData &data, const StabilityResult &stability);
    void buildPhaseChart(const BodeData &data, const StabilityResult &stability);

    QChartView *m_magView = nullptr;   ///< Ansicht des Amplitudendiagramms
    QChartView *m_phaseView = nullptr; ///< Ansicht des Phasendiagramms
};

#endif // BODEPLOTPANEL_H
