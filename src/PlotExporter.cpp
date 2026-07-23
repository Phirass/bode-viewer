#include "PlotExporter.h"

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QSize>
#include <QRectF>
#include <QSvgGenerator>
#include <QPrinter>
#include <QPageLayout>
#include <algorithm>

namespace {
// Feste Ausgabegroesse fuer den Export, unabhaengig von der Fenstergroesse,
// damit Bilder/Vektorgrafiken stets ein sauberes Seitenverhaeltnis haben.
constexpr int kExportWidth  = 1200;
constexpr int kExportHeight = 900;
}

bool PlotExporter::exportImage(QWidget *widget, const QString &filePath, QString *error)
{
    if (!widget) {
        if (error) *error = QStringLiteral("Kein Widget zum Exportieren vorhanden.");
        return false;
    }
    // Das Widget in seiner aktuellen Groesse als Pixmap erfassen.
    QPixmap pixmap = widget->grab();
    if (!pixmap.save(filePath)) {
        if (error) *error = QStringLiteral("Bild konnte nicht gespeichert werden: %1").arg(filePath);
        return false;
    }
    return true;
}

bool PlotExporter::exportSvg(QWidget *widget, const QString &filePath, QString *error)
{
    if (!widget) {
        if (error) *error = QStringLiteral("Kein Widget zum Exportieren vorhanden.");
        return false;
    }

    QSvgGenerator generator;
    generator.setFileName(filePath);
    generator.setSize(QSize(kExportWidth, kExportHeight));
    generator.setViewBox(QRect(0, 0, kExportWidth, kExportHeight));
    generator.setTitle(QStringLiteral("Bode-Diagramm"));
    generator.setDescription(QStringLiteral("Exportiert vom Bode-Diagramm-Viewer"));

    QPainter painter;
    if (!painter.begin(&generator)) {
        if (error) *error = QStringLiteral("SVG-Zeichenflaeche konnte nicht geoeffnet werden.");
        return false;
    }
    // Das Widget auf die feste Exportgroesse skalieren und zeichnen.
    const double sx = static_cast<double>(kExportWidth) / std::max(1, widget->width());
    const double sy = static_cast<double>(kExportHeight) / std::max(1, widget->height());
    painter.scale(sx, sy);
    widget->render(&painter);
    painter.end();
    return true;
}

bool PlotExporter::exportPdf(QWidget *widget, const QString &filePath, QString *error)
{
    if (!widget) {
        if (error) *error = QStringLiteral("Kein Widget zum Exportieren vorhanden.");
        return false;
    }

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filePath);

    QPainter painter;
    if (!painter.begin(&printer)) {
        if (error) *error = QStringLiteral("PDF konnte nicht erstellt werden: %1").arg(filePath);
        return false;
    }

    // Das Widget seitenfuellend und seitenverhaeltnistreu in die Seite einpassen.
    // paintRectPixels() liefert den bedruckbaren Bereich in Geraetepixeln.
    const QRectF page = printer.pageLayout().paintRectPixels(printer.resolution());
    const double scale = std::min(page.width()  / std::max(1, widget->width()),
                                  page.height() / std::max(1, widget->height()));
    painter.translate(page.topLeft());
    painter.scale(scale, scale);
    widget->render(&painter);
    painter.end();
    return true;
}
