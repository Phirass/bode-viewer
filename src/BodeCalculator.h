#ifndef BODECALCULATOR_H
#define BODECALCULATOR_H

#include <vector>
#include "TransferFunction.h"

/**
 * @brief Ergebnis einer Bode-Berechnung: drei parallele Felder gleicher Laenge.
 */
struct BodeData
{
    std::vector<double> omega;        ///< Kreisfrequenzen [rad/s] (logarithmisch verteilt)
    std::vector<double> magnitudeDb;  ///< Betrag in Dezibel: 20*log10(|G(j*omega)|)
    std::vector<double> phaseDeg;     ///< Phase in Grad (entfaltet / "unwrapped")

    bool isEmpty() const { return omega.empty(); }
    std::size_t size() const { return omega.size(); }
};

/**
 * @brief Berechnungsmodul (Kern): erzeugt aus einer Uebertragungsfunktion den
 *        Amplituden- und Phasenverlauf ueber einen Frequenzbereich.
 *
 * Die Frequenzen werden logarithmisch verteilt (gleichmaessige Punkte pro
 * Dekade), da die Frequenzachse eines Bode-Diagramms logarithmisch ist.
 */
class BodeCalculator
{
public:
    /**
     * @brief Berechnet den Frequenzgang.
     * @param tf      Auszuwertende Uebertragungsfunktion.
     * @param wMin    Untere Grenzfrequenz [rad/s], > 0.
     * @param wMax    Obere Grenzfrequenz [rad/s], > wMin.
     * @param points  Anzahl der Frequenzpunkte (>= 2).
     * @return Gefuellte BodeData-Struktur.
     */
    static BodeData compute(const TransferFunction &tf,
                            double wMin, double wMax, int points);

private:
    /**
     * @brief Entfaltet den Phasenverlauf ("phase unwrapping").
     *
     * std::arg liefert Werte in (-180, +180]. An den Sprungstellen wird ein
     * Vielfaches von 360 Grad addiert/subtrahiert, damit ein stetiger Verlauf
     * entsteht - Voraussetzung fuer eine korrekte Ablesung der Phasenreserve.
     */
    static void unwrapPhase(std::vector<double> &phaseDeg);
};

#endif // BODECALCULATOR_H
