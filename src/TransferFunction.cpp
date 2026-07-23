#include "TransferFunction.h"

#include <cmath>

TransferFunction::TransferFunction(std::vector<double> numerator,
                                   std::vector<double> denominator)
    : m_num(std::move(numerator)), m_den(std::move(denominator))
{
}

void TransferFunction::setNumerator(const std::vector<double> &numerator)
{
    m_num = numerator;
}

void TransferFunction::setDenominator(const std::vector<double> &denominator)
{
    m_den = denominator;
}

bool TransferFunction::isValid(QString *error) const
{
    if (m_num.empty()) {
        if (error) *error = QStringLiteral("Der Zaehler enthaelt keine Koeffizienten.");
        return false;
    }
    if (m_den.empty()) {
        if (error) *error = QStringLiteral("Der Nenner enthaelt keine Koeffizienten.");
        return false;
    }

    // Der Nenner muss mindestens einen von Null verschiedenen Koeffizienten
    // besitzen, sonst waere G(s) nicht definiert (Division durch das Nullpolynom).
    bool denNonZero = false;
    for (double c : m_den) {
        if (c != 0.0) { denNonZero = true; break; }
    }
    if (!denNonZero) {
        if (error) *error = QStringLiteral("Der Nenner darf nicht das Nullpolynom sein.");
        return false;
    }

    return true;
}

std::complex<double> TransferFunction::evaluatePolynomial(const std::vector<double> &coeffs,
                                                          const std::complex<double> &s)
{
    // Horner-Schema fuer absteigend geordnete Koeffizienten:
    //   p(s) = ((a_n * s + a_{n-1}) * s + ...) * s + a_0
    std::complex<double> result(0.0, 0.0);
    for (double c : coeffs) {
        result = result * s + std::complex<double>(c, 0.0);
    }
    return result;
}

std::complex<double> TransferFunction::evaluate(double omega) const
{
    // Fuer den Frequenzgang wird s = j*omega eingesetzt.
    const std::complex<double> s(0.0, omega);
    const std::complex<double> num = evaluatePolynomial(m_num, s);
    const std::complex<double> den = evaluatePolynomial(m_den, s);

    // Schutz vor Division durch (numerisch) Null: liefert einen sehr grossen
    // Wert zurueck, damit die Berechnung nicht mit NaN/Inf abbricht.
    if (std::abs(den) == 0.0) {
        return std::complex<double>(1e300, 0.0);
    }
    return num / den;
}

QString TransferFunction::polynomialToString(const std::vector<double> &coeffs)
{
    if (coeffs.empty())
        return QStringLiteral("0");

    const int degree = static_cast<int>(coeffs.size()) - 1;
    QString out;
    bool first = true;
    for (int i = 0; i < static_cast<int>(coeffs.size()); ++i) {
        const double c = coeffs[i];
        if (c == 0.0)
            continue;
        const int power = degree - i;
        if (!first)
            out += QStringLiteral(" + ");
        if (power == 0)
            out += QString::number(c);
        else if (power == 1)
            out += QStringLiteral("%1 s").arg(c);
        else
            out += QStringLiteral("%1 s^%2").arg(c).arg(power);
        first = false;
    }
    return first ? QStringLiteral("0") : out;
}

QString TransferFunction::toString() const
{
    return QStringLiteral("(%1) / (%2)")
        .arg(polynomialToString(m_num), polynomialToString(m_den));
}
