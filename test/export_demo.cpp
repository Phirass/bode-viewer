// ---------------------------------------------------------------------------
// Headless-Demo fuer den Diagramm-Export.
//
// Baut fuer das Referenzsystem G(s) = 1/(s(s+1)(s+2)) das Bode-Diagramm auf,
// fuellt das Plot-Panel und exportiert es in allen vier geforderten Formaten
// (PNG, JPG, SVG, PDF). Dient als reproduzierbarer Nachweis des Export-Moduls
// ohne manuelle Bedienung der GUI.
//
// Ausfuehrung ohne Bildschirm (z. B. Server/CI) moeglich mit:
//   QT_QPA_PLATFORM=offscreen ./BodeExportDemo
//
// Rueckgabewert: 0, wenn alle vier Exporte gelingen, sonst die Fehleranzahl.
// ---------------------------------------------------------------------------

#include "TransferFunction.h"
#include "BodeCalculator.h"
#include "StabilityAnalyzer.h"
#include "BodePlotPanel.h"
#include "PlotExporter.h"

#include <QApplication>
#include <QString>
#include <cstdio>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Referenzsystem aufbauen und Frequenzgang berechnen.
    TransferFunction tf({1.0}, {1.0, 3.0, 2.0, 0.0});
    const BodeData data = BodeCalculator::compute(tf, 0.01, 100.0, 800);
    const StabilityResult res = StabilityAnalyzer::analyze(data);

    // Plot-Panel mit Daten fuellen und auf eine feste Groesse bringen,
    // damit der Export unabhaengig vom Fenstermanager reproduzierbar ist.
    BodePlotPanel panel;
    panel.setData(data, res);
    panel.resize(900, 700);

    struct Job { const char *path; bool (*fn)(QWidget *, const QString &, QString *); };
    const Job jobs[] = {
        { "demo_bode.png", &PlotExporter::exportImage },
        { "demo_bode.jpg", &PlotExporter::exportImage },
        { "demo_bode.svg", &PlotExporter::exportSvg   },
        { "demo_bode.pdf", &PlotExporter::exportPdf   },
    };

    int failures = 0;
    for (const Job &j : jobs) {
        QString error;
        if (j.fn(&panel, QString::fromUtf8(j.path), &error)) {
            std::printf("  [OK]   Export %s\n", j.path);
        } else {
            std::printf("  [FEHL] Export %s: %s\n", j.path, error.toUtf8().constData());
            ++failures;
        }
    }

    std::printf("\n=== Export-Demo: %d von %d Formaten erfolgreich ===\n",
                4 - failures, 4);
    return failures;
}
