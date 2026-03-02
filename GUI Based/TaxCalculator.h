#ifndef TAXCALCULATOR_H
#define TAXCALCULATOR_H

#include "taxpayerinfo.h"

struct RegimeResult
{
    double totalTax = 0;
    double netSalary = 0;
    double normalTaxable = 0;
    double totalDed = 0;
    double slabTax = 0;
    double capTax = 0;
    double rebate = 0;
    double surchargeAmt = 0;
    double cess = 0;
};

class TaxCalculator
{
public:
    explicit TaxCalculator(const TaxPayerInfo& info);

    RegimeResult calculateOldRegime() const;
    RegimeResult calculateNewRegime() const;

private:
    TaxPayerInfo tp;

    double grossSalary()    const;
    double hraExemption()   const;
    double capitalGainsTax() const;

    double oldRegimeSlabTax(double taxable) const;
    double newRegimeSlabTax(double taxable) const;
    double surcharge(double tax, double totalTaxableIncome, bool isNewRegime) const;
    double rebate87A(double normalTaxable, double slabTax, bool isNewRegime) const;
};

#endif // TAXCALCULATOR_H