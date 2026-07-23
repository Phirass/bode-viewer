#include "StabilityAnalyzer.h"

#include <cmath>

namespace {

/**
 * @brief Interpoliert die Frequenz logarithmisch zwischen zwei Stuetzstellen.
 * @param w0,w1  benachbarte Frequenzen
 * @param t      Interpolationsfaktor in [0,1]
 */
double interpFreqLog(double w0, double w1, double t)
{
    const double logW = std::log10(w0) + t * (std::log10(w1) - std::log10(w0));
    return std::pow(10.0, logW);
}

} // namespace

StabilityResult StabilityAnalyzer::analyze(const BodeData &data)
{
    StabilityResult r;
    if (data.size() < 2)
        return r;

    const auto &w   = data.omega;
    const auto &mag = data.magnitudeDb;
    const auto &ph  = data.phaseDeg;

    // --- Amplitudendurchtrittsfrequenz: erster Nulldurchgang des Betrags (0 dB) ---
    // Dort wird der Phasenrand PM = 180 + Phase(omega_gc) abgelesen.
    for (std::size_t i = 1; i < data.size(); ++i) {
        const double a = mag[i - 1];
        const double b = mag[i];
        if ((a == 0.0) || (a < 0.0) != (b < 0.0)) {
            const double denom = (b - a);
            const double t = (denom != 0.0) ? (0.0 - a) / denom : 0.0;
            r.gainCrossoverW = interpFreqLog(w[i - 1], w[i], t);
            const double phaseAtGc = ph[i - 1] + t * (ph[i] - ph[i - 1]);
            r.phaseMarginDeg = 180.0 + phaseAtGc;
            r.hasPhaseMargin = true;
            break;
        }
    }

    // --- Phasendurchtrittsfrequenz: erster Durchgang der Phase durch -180 Grad ---
    // Dort wird der Amplitudenrand GM = -Betrag(omega_pc) [dB] abgelesen.
    for (std::size_t i = 1; i < data.size(); ++i) {
        const double a = ph[i - 1] + 180.0;
        const double b = ph[i] + 180.0;
        if ((a == 0.0) || (a < 0.0) != (b < 0.0)) {
            const double denom = (b - a);
            const double t = (denom != 0.0) ? (0.0 - a) / denom : 0.0;
            r.phaseCrossoverW = interpFreqLog(w[i - 1], w[i], t);
            const double magAtPc = mag[i - 1] + t * (mag[i] - mag[i - 1]);
            r.gainMarginDb = -magAtPc;   // Abstand zu 0 dB
            r.hasGainMargin = true;
            break;
        }
    }

    // --- Gesamtbewertung ---------------------------------------------------
    // Vereinfachtes Kriterium fuer offene Regelkreise mit einem Durchtritt:
    // Das System ist stabil, wenn Phasenrand UND Amplitudenrand positiv sind.
    // Findet die Phase keinen -180-Grad-Durchgang, ist der Amplitudenrand
    // unendlich gross; dann entscheidet allein der Phasenrand.
    if (r.hasPhaseMargin) {
        const bool gainOk = r.hasGainMargin ? (r.gainMarginDb > 0.0) : true;
        r.stable = (r.phaseMarginDeg > 0.0) && gainOk;
    }

    return r;
}

QString StabilityResult::summary() const
{
    QString s;

    if (hasPhaseMargin)
        s += QStringLiteral("Phasenrand: %1 Grad  bei  %2 rad/s\n")
                 .arg(phaseMarginDeg, 0, 'f', 2)
                 .arg(gainCrossoverW, 0, 'g', 4);
    else
        s += QStringLiteral("Phasenrand: kein Amplitudendurchgang (0 dB) im Bereich\n");

    if (hasGainMargin)
        s += QStringLiteral("Amplitudenrand: %1 dB  bei  %2 rad/s\n")
                 .arg(gainMarginDb, 0, 'f', 2)
                 .arg(phaseCrossoverW, 0, 'g', 4);
    else
        s += QStringLiteral("Amplitudenrand: kein Phasendurchgang (-180 Grad) -> unendlich\n");

    if (hasPhaseMargin)
        s += stable ? QStringLiteral("Bewertung: STABIL")
                    : QStringLiteral("Bewertung: INSTABIL");
    else
        s += QStringLiteral("Bewertung: nicht bestimmbar (Bereich anpassen)");

    return s;
}
