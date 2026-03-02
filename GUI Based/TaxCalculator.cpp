#include "pch.h"
#include "taxcalculator.h"
#include <algorithm>
using std::min;

TaxCalculator::TaxCalculator(const TaxPayerInfo& info)
    : tp(info)
{
}

// ─── Private helpers ──────────────────────────────────────────────────────────

double TaxCalculator::grossSalary() const
{
    return tp.BasicSalary + tp.HRAReceived + tp.SpecialAllowance + tp.Bonus;
}

double TaxCalculator::hraExemption() const
{
    if (tp.MonthlyRentPaid <= 0)
        return 0;

    double annualRent = tp.MonthlyRentPaid * 12;
    double cityPct = tp.IsMetroCity ? 0.50 : 0.40;

    double iNo1 = tp.HRAReceived;
    double iNo2 = cityPct * tp.BasicSalary;
    double iNo3 = annualRent - 0.10 * tp.BasicSalary;

    double exemption = min({ iNo1, iNo2, iNo3 });
    return exemption < 0 ? 0 : exemption;
}

double TaxCalculator::oldRegimeSlabTax(double taxable) const
{
    double exempt = 250000;
    if (tp.Age >= 60 && tp.Age < 80) exempt = 300000;
    if (tp.Age >= 80)                exempt = 500000;

    if (taxable <= exempt) return 0;

    double tax = (min(taxable, 500000.0) - exempt) * 0.05;

    if (taxable > 500000)
        tax += (min(taxable, 1000000.0) - 500000.0) * 0.20;

    if (taxable > 1000000)
        tax += (taxable - 1000000.0) * 0.30;

    return tax;
}

double TaxCalculator::newRegimeSlabTax(double taxable) const
{
    if (taxable <= 300000) return 0;

    double tax = (min(taxable, 700000.0) - 300000.0) * 0.05;

    if (taxable > 700000)
        tax += (min(taxable, 1000000.0) - 700000.0) * 0.10;

    if (taxable > 1000000)
        tax += (min(taxable, 1200000.0) - 1000000.0) * 0.15;

    if (taxable > 1200000)
        tax += (min(taxable, 1500000.0) - 1200000.0) * 0.20;

    if (taxable > 1500000)
        tax += (taxable - 1500000.0) * 0.30;

    return tax;
}

double TaxCalculator::capitalGainsTax() const
{
    double stcgTax = (tp.STCG > 0) ? tp.STCG * 0.15 : 0;
    double ltcgTax = (tp.LTCG > 100000) ? (tp.LTCG - 100000) * 0.10 : 0;
    return stcgTax + ltcgTax;
}

double TaxCalculator::surcharge(double tax, double totalTaxableIncome, bool isNewRegime) const
{
    double rate = 0;
    if (totalTaxableIncome > 5000000 && totalTaxableIncome <= 10000000) rate = 0.10;
    if (totalTaxableIncome > 10000000 && totalTaxableIncome <= 20000000) rate = 0.15;
    if (totalTaxableIncome > 20000000 && totalTaxableIncome <= 50000000) rate = 0.25;
    if (totalTaxableIncome > 50000000) rate = isNewRegime ? 0.25 : 0.37;
    return tax * rate;
}

double TaxCalculator::rebate87A(double normalTaxable, double slabTax, bool isNewRegime) const
{
    if (!isNewRegime && normalTaxable <= 500000) return min(slabTax, 12500.0);
    if (isNewRegime && normalTaxable <= 700000) return min(slabTax, 25000.0);
    return 0;
}

// ─── Public calculations ──────────────────────────────────────────────────────

RegimeResult TaxCalculator::calculateOldRegime() const
{
    RegimeResult r;

    r.netSalary = grossSalary() - 50000 - hraExemption();
    if (r.netSalary < 0) r.netSalary = 0;

    double ded80C = min(tp.Sec80C, 150000.0);
    double dedHL = min(tp.HomeLoanInterest, 200000.0);
    r.totalDed = ded80C + tp.Sec80D + tp.Sec80E + dedHL + tp.OtherDeductions;

    r.normalTaxable = r.netSalary + tp.RentalIncome + tp.OtherIncome - r.totalDed;
    if (r.normalTaxable < 0) r.normalTaxable = 0;

    r.slabTax = oldRegimeSlabTax(r.normalTaxable);
    r.capTax = capitalGainsTax();
    r.rebate = rebate87A(r.normalTaxable, r.slabTax, false);

    double taxAfter = qMax(0.0, r.slabTax - r.rebate) + r.capTax;

    double totalTaxable = r.normalTaxable + tp.STCG + tp.LTCG;
    r.surchargeAmt = surcharge(taxAfter, totalTaxable, false);
    r.cess = (taxAfter + r.surchargeAmt) * 0.04;
    r.totalTax = taxAfter + r.surchargeAmt + r.cess;

    return r;
}

RegimeResult TaxCalculator::calculateNewRegime() const
{
    RegimeResult r;

    r.netSalary = grossSalary() - 75000;
    if (r.netSalary < 0) r.netSalary = 0;

    r.totalDed = tp.EmployerNPS;
    r.normalTaxable = r.netSalary + tp.RentalIncome + tp.OtherIncome - tp.EmployerNPS;
    if (r.normalTaxable < 0) r.normalTaxable = 0;

    r.slabTax = newRegimeSlabTax(r.normalTaxable);
    r.capTax = capitalGainsTax();
    r.rebate = rebate87A(r.normalTaxable, r.slabTax, true);

    double taxAfter = qMax(0.0, r.slabTax - r.rebate) + r.capTax;

    double totalTaxable = r.normalTaxable + tp.STCG + tp.LTCG;
    r.surchargeAmt = surcharge(taxAfter, totalTaxable, true);
    r.cess = (taxAfter + r.surchargeAmt) * 0.04;
    r.totalTax = taxAfter + r.surchargeAmt + r.cess;

    return r;
}