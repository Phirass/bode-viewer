#ifndef TRANSFERFUNCTION_H
#define TRANSFERFUNCTION_H

#include <vector>
#include <complex>
#include <QString>

/**
 * @brief Modell-Modul: Repraesentiert eine generische Uebertragungsfunktion
 *        G(s) = Zaehlerpolynom(s) / Nennerpolynom(s).
 *
 * Die Koeffizienten werden in ABSTEIGENDER Potenz gespeichert, d. h.
 * das Feld {a_n, a_{n-1}, ..., a_1, a_0} beschreibt das Polynom
 *   a_n * s^n + a_{n-1} * s^{n-1} + ... + a_1 * s + a_0.
 *
 * Beispiel:  Zaehler {1}, Nenner {1, 3, 2, 0}  ->  G(s) = 1 / (s^3 + 3 s^2 + 2 s).
 *
 * Das Modul kapselt ausschliesslich die Daten der Uebertragungsfunktion und
 * deren Auswertung. Es kennt weder die GUI noch die Darstellung (klare
 * Trennung von Logik und Praesentation).
 */
class TransferFunction
{
public:
    TransferFunction() = default;
    TransferFunction(std::vector<double> numerator, std::vector<double> denominator);

    void setNumerator(const std::vector<double> &numerator);
    void setDenominator(const std::vector<double> &denominator);

    const std::vector<double> &numerator() const { return m_num; }
    const std::vector<double> &denominator() const { return m_den; }

    /**
     * @brief Prueft, ob die Uebertragungsfunktion gueltig ist.
     * @param error  Optional: erhaelt bei Ungueltigkeit eine Fehlermeldung.
     * @return true, wenn Zaehler und Nenner nicht leer sind und der Nenner
     *         mindestens einen von Null verschiedenen Koeffizienten besitzt.
     */
    bool isValid(QString *error = nullptr) const;

    /**
     * @brief Wertet G(j*omega) aus, indem s = j*omega gesetzt wird.
     * @param omega  Kreisfrequenz in rad/s.
     * @return Komplexer Frequenzgangwert G(j*omega).
     */
    std::complex<double> evaluate(double omega) const;

    /** @brief Liefert eine lesbare Darstellung, z. B. "(1) / (1 s^3 + 3 s^2 + 2 s)". */
    QString toString() const;

private:
    /** @brief Wertet ein Polynom (Koeffizienten absteigend) per Horner-Schema aus. */
    static std::complex<double> evaluatePolynomial(const std::vector<double> &coeffs,
                                                   const std::complex<double> &s);
    static QString polynomialToString(const std::vector<double> &coeffs);

    std::vector<double> m_num{1.0};   ///< Zaehlerkoeffizienten (absteigend)
    std::vector<double> m_den{1.0};   ///< Nennerkoeffizienten (absteigend)
};

#endif // TRANSFERFUNCTION_H
