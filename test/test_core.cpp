// ---------------------------------------------------------------------------
// Konsolen-Test der Kernmodule (Modell, Berechnung, Stabilitaet).
//
// Der Test prueft die Kernlogik gegen analytisch bekannte Werte und ist
// unabhaengig von der grafischen Oberflaeche. Er wird ueber die CMake-Option
// -DBUILD_TESTS=ON gebaut und erzeugt das Programm "BodeCoreTest".
//
// Rueckgabewert: 0, wenn alle Pruefungen bestehen, sonst die Anzahl der Fehler.
// ---------------------------------------------------------------------------

#include "TransferFunction.h"
#include "BodeCalculator.h"
#include "StabilityAnalyzer.h"

#include <cmath>
#include <complex>
#include <cstdio>
#include <string>

namespace {

// Zaehler bestandener und fehlgeschlagener Pruefungen.
int g_passed = 0;
int g_failed = 0;

// Prueft, ob zwei Gleitkommazahlen innerhalb einer Toleranz uebereinstimmen.
void checkClose(const std::string &name, double actual, double expected, double tol)
{
    const double diff = std::fabs(actual - expected);
    if (diff <= tol) {
        std::printf("  [OK]   %-40s berechnet=%.4f  erwartet=%.4f\n",
                    name.c_str(), actual, expected);
        ++g_passed;
    } else {
        std::printf("  [FEHL] %-40s berechnet=%.4f  erwartet=%.4f  (Abw=%.4f)\n",
                    name.c_str(), actual, expected, diff);
        ++g_failed;
    }
}

} // namespace

int main()
{
    std::printf("=== Kern-Test: Bode-Diagramm-Viewer ===\n\n");

    // -----------------------------------------------------------------------
    // Test 1: Direkte Auswertung von G(j*omega) fuer G(s) = 1 / (s + 1).
    // Analytisch gilt bei omega = 1:  G(j1) = 1/(1 + j) = 0.5 - 0.5 j,
    // Betrag = 1/sqrt(2)  ->  -3.0103 dB,  Phase = -45 Grad.
    // -----------------------------------------------------------------------
    std::printf("Test 1: G(s) = 1/(s+1) bei omega = 1 rad/s\n");
    {
        TransferFunction tf({1.0}, {1.0, 1.0});
        const std::complex<double> g = tf.evaluate(1.0);
        const double magDb = 20.0 * std::log10(std::abs(g));
        const double phase = std::arg(g) * 180.0 / 3.14159265358979323846;

        checkClose("Realteil G(j1)",        g.real(),  0.5,     1e-6);
        checkClose("Imaginaerteil G(j1)",   g.imag(), -0.5,     1e-6);
        checkClose("Betrag [dB]",           magDb,    -3.0103,  1e-3);
        checkClose("Phase [Grad]",          phase,    -45.0,    1e-3);
    }

    // -----------------------------------------------------------------------
    // Test 2: Vollstaendige Bode- und Stabilitaetsberechnung fuer das
    // Referenzsystem G(s) = 1 / (s (s+1)(s+2)).
    // Nenner ausmultipliziert:  s^3 + 3 s^2 + 2 s  ->  {1, 3, 2, 0}.
    //
    // Analytische Vergleichswerte:
    //   Phasendurchtritt   omega_pc = sqrt(2) = 1.4142 rad/s
    //   Amplitudenrand     GM       = 15.56 dB
    //   Amplitudendurchtritt omega_gc = 0.446 rad/s
    //   Phasenrand         PM       = 53.4 Grad
    // -----------------------------------------------------------------------
    std::printf("\nTest 2: Referenzsystem G(s) = 1/(s(s+1)(s+2))\n");
    {
        TransferFunction tf({1.0}, {1.0, 3.0, 2.0, 0.0});

        // Feines, logarithmisch verteiltes Frequenzraster fuer eine genaue
        // Interpolation der Durchtrittsfrequenzen.
        const BodeData data = BodeCalculator::compute(tf, 0.01, 100.0, 2000);
        const StabilityResult res = StabilityAnalyzer::analyze(data);

        checkClose("Phasendurchtritt omega_pc [rad/s]",     res.phaseCrossoverW, 1.4142, 1e-2);
        checkClose("Amplitudenrand GM [dB]",                res.gainMarginDb,    15.56,  0.1);
        checkClose("Amplitudendurchtritt omega_gc [rad/s]", res.gainCrossoverW,  0.446,  1e-2);
        checkClose("Phasenrand PM [Grad]",                  res.phaseMarginDeg,  53.4,   0.2);
    }

    std::printf("\n=== Ergebnis: %d bestanden, %d fehlgeschlagen ===\n",
                g_passed, g_failed);

    return g_failed;   // 0 = alle Tests bestanden
}
