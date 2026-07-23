#ifndef STABILITYANALYZER_H
#define STABILITYANALYZER_H

#include <QString>
#include "BodeCalculator.h"

/**
 * @brief Ergebnis der Stabilitaetsbewertung.
 */
struct StabilityResult
{
    // Amplitudenrand (gain margin): Abstand des Betrags zu 0 dB bei der
    // Phasendurchtrittsfrequenz (dort, wo die Phase -180 Grad erreicht).
    bool   hasGainMargin       = false;
    double gainMarginDb        = 0.0;   ///< GM in dB (positiv = stabilitaetsreserve)
    double phaseCrossoverW     = 0.0;   ///< omega bei Phase = -180 Grad [rad/s]

    // Phasenrand (phase margin): Abstand der Phase zu -180 Grad bei der
    // Amplitudendurchtrittsfrequenz (dort, wo der Betrag 0 dB erreicht).
    bool   hasPhaseMargin      = false;
    double phaseMarginDeg      = 0.0;   ///< PM in Grad (positiv = stabilitaetsreserve)
    double gainCrossoverW      = 0.0;   ///< omega bei Betrag = 0 dB [rad/s]

    bool   stable              = false; ///< Gesamtbewertung (PM > 0 und GM > 0)

    /** @brief Liefert eine kurze, lesbare Zusammenfassung in deutscher Sprache. */
    QString summary() const;
};

/**
 * @brief Stabilitaetsmodul: bestimmt Amplituden- und Phasenrand aus einem
 *        bereits berechneten Bode-Datensatz.
 *
 * Die Durchtrittsfrequenzen liegen im Allgemeinen zwischen zwei berechneten
 * Stuetzstellen. Sie werden durch lineare Interpolation ermittelt - in der
 * Frequenz logarithmisch (passend zur log. Achse), in den Werten linear.
 * Es wird jeweils der erste Nulldurchgang ausgewertet.
 */
class StabilityAnalyzer
{
public:
    static StabilityResult analyze(const BodeData &data);
};

#endif // STABILITYANALYZER_H
