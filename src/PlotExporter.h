#ifndef PLOTEXPORTER_H
#define PLOTEXPORTER_H

#include <QString>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

/**
 * @brief Export-Modul: speichert ein beliebiges Widget (hier das Plot-Panel)
 *        in Raster- oder Vektorformate.
 *
 * - Raster (PNG/JPG): das Widget wird per QWidget::grab() als Pixmap erfasst.
 * - SVG: ueber QSvgGenerator als Zeichenflaeche (verlustfreie Vektorgrafik).
 * - PDF: ueber QPrinter im PDF-Modus (vektoriell, seitenfuellend skaliert).
 *
 * Alle Funktionen liefern true bei Erfolg, sonst false (mit optionaler
 * Fehlermeldung).
 */
namespace PlotExporter
{
/** @brief Speichert als PNG oder JPG (Format anhand der Dateiendung). */
bool exportImage(QWidget *widget, const QString &filePath, QString *error = nullptr);

/** @brief Speichert als SVG-Vektorgrafik. */
bool exportSvg(QWidget *widget, const QString &filePath, QString *error = nullptr);

/** @brief Speichert als PDF-Vektordokument. */
bool exportPdf(QWidget *widget, const QString &filePath, QString *error = nullptr);
}

#endif // PLOTEXPORTER_H
