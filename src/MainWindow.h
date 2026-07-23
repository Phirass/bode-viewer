#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QSpinBox;
class QLabel;
class QPushButton;
QT_END_NAMESPACE

class BodePlotPanel;

/**
 * @brief GUI-Modul: Hauptfenster und Steuerzentrale der Anwendung.
 *
 * Das Fenster nimmt die Eingaben entgegen (Zaehler-/Nennerkoeffizienten,
 * Frequenzbereich, Punktzahl), stoesst ueber Signals/Slots die Berechnung an
 * und zeigt Bode-Diagramm sowie Stabilitaetsraender an. Ueber das Menue
 * "Datei" koennen die Diagramme exportiert werden.
 *
 * Die eigentliche Fachlogik liegt in den Modulen TransferFunction,
 * BodeCalculator, StabilityAnalyzer, BodePlotPanel und PlotExporter; das
 * GUI-Modul koordiniert lediglich deren Zusammenspiel.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onCompute();          ///< liest Eingaben, berechnet und zeichnet
    void onExportPng();
    void onExportJpg();
    void onExportSvg();
    void onExportPdf();

private:
    void createMenu();
    void createControls();

    /** @brief Zerlegt einen Text ("1, 3, 2, 0" oder "1 3 2 0") in Koeffizienten. */
    static bool parseCoefficients(const QString &text,
                                  std::vector<double> &out, QString *error);

    /** @brief Gemeinsame Export-Routine mit Dateidialog. */
    void exportWithDialog(const QString &kind);

    // Eingabe-Widgets
    QLineEdit    *m_numEdit   = nullptr;   ///< Zaehlerkoeffizienten
    QLineEdit    *m_denEdit   = nullptr;   ///< Nennerkoeffizienten
    QLineEdit    *m_wMinEdit  = nullptr;   ///< untere Frequenz [rad/s]
    QLineEdit    *m_wMaxEdit  = nullptr;   ///< obere Frequenz [rad/s]
    QSpinBox     *m_pointsBox = nullptr;   ///< Anzahl Frequenzpunkte
    QPushButton  *m_computeBtn = nullptr;

    // Ausgabe-Widgets
    QLabel        *m_tfLabel      = nullptr; ///< zeigt G(s) im Klartext
    QLabel        *m_stabilityLabel = nullptr;
    BodePlotPanel *m_plot         = nullptr;
};

#endif // MAINWINDOW_H
