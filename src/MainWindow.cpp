#include "MainWindow.h"

#include "TransferFunction.h"
#include "BodeCalculator.h"
#include "StabilityAnalyzer.h"
#include "BodePlotPanel.h"
#include "PlotExporter.h"

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>

#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Bode-Diagramm-Viewer"));
    resize(1100, 800);

    createMenu();
    createControls();

    // Beim Start direkt ein Beispiel berechnen und anzeigen.
    onCompute();
}

void MainWindow::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(QStringLiteral("&Datei"));

    QAction *pngAction = fileMenu->addAction(QStringLiteral("Export als &PNG ..."));
    QAction *jpgAction = fileMenu->addAction(QStringLiteral("Export als &JPG ..."));
    QAction *svgAction = fileMenu->addAction(QStringLiteral("Export als &SVG ..."));
    QAction *pdfAction = fileMenu->addAction(QStringLiteral("Export als P&DF ..."));
    fileMenu->addSeparator();
    QAction *quitAction = fileMenu->addAction(QStringLiteral("&Beenden"));

    connect(pngAction,  &QAction::triggered, this, &MainWindow::onExportPng);
    connect(jpgAction,  &QAction::triggered, this, &MainWindow::onExportJpg);
    connect(svgAction,  &QAction::triggered, this, &MainWindow::onExportSvg);
    connect(pdfAction,  &QAction::triggered, this, &MainWindow::onExportPdf);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);
}

void MainWindow::createControls()
{
    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    // --- Eingabebereich ----------------------------------------------------
    auto *inputGroup = new QGroupBox(QStringLiteral("Uebertragungsfunktion G(s) = Zaehler / Nenner"), central);
    auto *form = new QFormLayout(inputGroup);

    m_numEdit  = new QLineEdit(QStringLiteral("1"), inputGroup);
    m_denEdit  = new QLineEdit(QStringLiteral("1 3 2 0"), inputGroup);
    m_wMinEdit = new QLineEdit(QStringLiteral("0.01"), inputGroup);
    m_wMaxEdit = new QLineEdit(QStringLiteral("1000"), inputGroup);
    m_pointsBox = new QSpinBox(inputGroup);
    m_pointsBox->setRange(2, 100000);
    m_pointsBox->setValue(600);

    m_numEdit->setToolTip(QStringLiteral("Koeffizienten absteigend, z. B. \"1\" oder \"2 5\""));
    m_denEdit->setToolTip(QStringLiteral("Koeffizienten absteigend, z. B. \"1 3 2 0\" fuer s^3+3s^2+2s"));

    form->addRow(QStringLiteral("Zaehler-Koeffizienten:"), m_numEdit);
    form->addRow(QStringLiteral("Nenner-Koeffizienten:"), m_denEdit);
    form->addRow(QStringLiteral("Frequenz min [rad/s]:"), m_wMinEdit);
    form->addRow(QStringLiteral("Frequenz max [rad/s]:"), m_wMaxEdit);
    form->addRow(QStringLiteral("Anzahl Punkte:"), m_pointsBox);

    m_computeBtn = new QPushButton(QStringLiteral("Bode-Diagramm berechnen"), inputGroup);
    form->addRow(m_computeBtn);
    connect(m_computeBtn, &QPushButton::clicked, this, &MainWindow::onCompute);

    // --- Ausgabebereich (Text) --------------------------------------------
    m_tfLabel = new QLabel(central);
    m_tfLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_stabilityLabel = new QLabel(central);
    m_stabilityLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_stabilityLabel->setStyleSheet(QStringLiteral("font-family: monospace;"));

    // --- Diagramm ----------------------------------------------------------
    m_plot = new BodePlotPanel(central);
    m_plot->setMinimumHeight(400);

    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(m_tfLabel);
    mainLayout->addWidget(m_stabilityLabel);
    mainLayout->addWidget(m_plot, /*stretch*/ 1);

    setCentralWidget(central);
    statusBar()->showMessage(QStringLiteral("Bereit."));
}

bool MainWindow::parseCoefficients(const QString &text,
                                   std::vector<double> &out, QString *error)
{
    out.clear();
    // An Komma, Semikolon oder Leerraum trennen.
    const QStringList tokens =
        text.split(QRegularExpression(QStringLiteral("[\\s,;]+")), Qt::SkipEmptyParts);

    if (tokens.isEmpty()) {
        if (error) *error = QStringLiteral("Es wurden keine Koeffizienten eingegeben.");
        return false;
    }
    for (const QString &tok : tokens) {
        bool ok = false;
        const double value = tok.toDouble(&ok);
        if (!ok) {
            if (error) *error = QStringLiteral("Ungueltige Zahl: \"%1\"").arg(tok);
            return false;
        }
        out.push_back(value);
    }
    return true;
}

void MainWindow::onCompute()
{
    // 1) Eingaben einlesen und validieren.
    std::vector<double> num, den;
    QString error;
    if (!parseCoefficients(m_numEdit->text(), num, &error)) {
        QMessageBox::warning(this, QStringLiteral("Fehler im Zaehler"), error);
        return;
    }
    if (!parseCoefficients(m_denEdit->text(), den, &error)) {
        QMessageBox::warning(this, QStringLiteral("Fehler im Nenner"), error);
        return;
    }

    bool okMin = false, okMax = false;
    const double wMin = m_wMinEdit->text().toDouble(&okMin);
    const double wMax = m_wMaxEdit->text().toDouble(&okMax);
    if (!okMin || !okMax || wMin <= 0.0 || wMax <= wMin) {
        QMessageBox::warning(this, QStringLiteral("Fehler im Frequenzbereich"),
                             QStringLiteral("Bitte 0 < Frequenz min < Frequenz max angeben."));
        return;
    }

    // 2) Modell erzeugen und pruefen.
    TransferFunction tf(num, den);
    if (!tf.isValid(&error)) {
        QMessageBox::warning(this, QStringLiteral("Ungueltige Uebertragungsfunktion"), error);
        return;
    }

    // 3) Berechnung + Stabilitaetsanalyse (Kern-Module).
    const BodeData data = BodeCalculator::compute(tf, wMin, wMax, m_pointsBox->value());
    const StabilityResult stability = StabilityAnalyzer::analyze(data);

    // 4) Darstellung aktualisieren.
    m_plot->setData(data, stability);
    m_tfLabel->setText(QStringLiteral("G(s) = ") + tf.toString());
    m_stabilityLabel->setText(stability.summary());
    statusBar()->showMessage(QStringLiteral("Berechnung abgeschlossen (%1 Punkte).")
                                 .arg(data.size()));
}

void MainWindow::exportWithDialog(const QString &kind)
{
    QString filter, defaultExt;
    if (kind == QLatin1String("png"))      { filter = QStringLiteral("PNG-Bild (*.png)");  defaultExt = QStringLiteral(".png"); }
    else if (kind == QLatin1String("jpg")) { filter = QStringLiteral("JPG-Bild (*.jpg)");  defaultExt = QStringLiteral(".jpg"); }
    else if (kind == QLatin1String("svg")) { filter = QStringLiteral("SVG-Grafik (*.svg)"); defaultExt = QStringLiteral(".svg"); }
    else                                   { filter = QStringLiteral("PDF-Dokument (*.pdf)"); defaultExt = QStringLiteral(".pdf"); }

    QString path = QFileDialog::getSaveFileName(
        this, QStringLiteral("Diagramm exportieren"),
        QStringLiteral("bode-diagramm") + defaultExt, filter);
    if (path.isEmpty())
        return;   // vom Benutzer abgebrochen
    if (!path.endsWith(defaultExt, Qt::CaseInsensitive))
        path += defaultExt;

    QString error;
    bool ok = false;
    if (kind == QLatin1String("png") || kind == QLatin1String("jpg"))
        ok = PlotExporter::exportImage(m_plot, path, &error);
    else if (kind == QLatin1String("svg"))
        ok = PlotExporter::exportSvg(m_plot, path, &error);
    else
        ok = PlotExporter::exportPdf(m_plot, path, &error);

    if (ok)
        statusBar()->showMessage(QStringLiteral("Exportiert nach: %1").arg(path));
    else
        QMessageBox::critical(this, QStringLiteral("Export fehlgeschlagen"), error);
}

void MainWindow::onExportPng() { exportWithDialog(QStringLiteral("png")); }
void MainWindow::onExportJpg() { exportWithDialog(QStringLiteral("jpg")); }
void MainWindow::onExportSvg() { exportWithDialog(QStringLiteral("svg")); }
void MainWindow::onExportPdf() { exportWithDialog(QStringLiteral("pdf")); }
