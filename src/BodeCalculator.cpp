#include "BodeCalculator.h"

#include <algorithm>
#include <cmath>

// Pi portabel definieren (MSVC stellt M_PI ohne _USE_MATH_DEFINES nicht bereit).
namespace {
constexpr double kPi = 3.14159265358979323846;
}

BodeData BodeCalculator::compute(const TransferFunction &tf,
                                 double wMin, double wMax, int points)
{
    BodeData data;

    // Robustheit: gueltigen Frequenzbereich und Punktzahl erzwingen.
    if (points < 2)
        points = 2;
    if (wMin <= 0.0)
        wMin = 1e-3;              // log10 ist nur fuer positive Frequenzen definiert
    if (wMax <= wMin)
        wMax = wMin * 10.0;

    data.omega.reserve(points);
    data.magnitudeDb.reserve(points);
    data.phaseDeg.reserve(points);

    const double logMin = std::log10(wMin);
    const double logMax = std::log10(wMax);
    const double step = (logMax - logMin) / (points - 1);

    const double radToDeg = 180.0 / kPi;

    for (int i = 0; i < points; ++i) {
        // Logarithmisch verteilte Frequenz: omega = 10^(logMin + i*step)
        const double omega = std::pow(10.0, logMin + i * step);
        const std::complex<double> g = tf.evaluate(omega);

        const double mag = std::abs(g);
        // Betrag in dB; sehr kleine Betraege werden begrenzt, um -inf zu vermeiden.
        const double magDb = 20.0 * std::log10(std::max(mag, 1e-30));
        const double phase = std::arg(g) * radToDeg;

        data.omega.push_back(omega);
        data.magnitudeDb.push_back(magDb);
        data.phaseDeg.push_back(phase);
    }

    unwrapPhase(data.phaseDeg);
    return data;
}

void BodeCalculator::unwrapPhase(std::vector<double> &phaseDeg)
{
    if (phaseDeg.size() < 2)
        return;

    double offset = 0.0;
    for (std::size_t i = 1; i < phaseDeg.size(); ++i) {
        // Differenz zum bereits korrigierten Vorgaengerwert betrachten.
        double diff = (phaseDeg[i] + offset) - phaseDeg[i - 1];
        while (diff > 180.0) {
            offset -= 360.0;
            diff -= 360.0;
        }
        while (diff < -180.0) {
            offset += 360.0;
            diff += 360.0;
        }
        phaseDeg[i] += offset;
    }
}
