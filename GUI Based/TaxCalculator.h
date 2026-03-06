#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  TaxCalculator.h  –  Pure C++ tax logic (no Qt, no iostream)
//  FY 2024-25 | Old Regime & New Regime
// ─────────────────────────────────────────────────────────────────────────────

struct TaxPayerIncome
{
    // Personal
    char   TaxPayerName[100];
    int    Age;
    int    FinancialYear;

    // Salary (Annual)
    double Basic;
    double HRA;
    double SpecialAllowance;
    double Bonus;
    double OtherSalary;

    // House Property
    double GrossRental;
    double MunicipalTax;

    // Capital Gains
    double STCG;   // @15%
    double LTCG;   // @10% above 1 lakh

    // Other Income
    double InterestIncome;
    double BusinessIncome;
    double OtherIncome;

    // Deductions – Old Regime
    double Sec80C;          // max 1,50,000
    double Sec80D;
    double Sec80E;
    double Sec80G;
    double Sec80TTA;        // max 10,000
    double Sec80CCD1B;      // max 50,000
    double HomeLoanInt;     // Sec 24b, max 2,00,000
    double HRAExemption;    // auto-calculated
    double LTAExemption;
    double OtherDeductions;

    // Deductions – New Regime only
    double EmployerNPS;
    double AgniVeer;

    // HRA helper inputs
    double MonthlyRentPaid;
    bool   IsMetroCity;
};

// ─── Result bundle ────────────────────────────────────────────────────────────
struct TaxResult
{
    double grossIncome;
    double standardDeduction;
    double totalDeductions;
    double normalTaxable;
    double taxableIncome;       // includes capital gains
    double slabTax;
    double stcgTax;
    double ltcgTax;
    double taxBeforeRebate;
    double rebate87A;
    double taxAfterRebate;
    double surcharge;
    double cess;
    double totalTax;
    double effectiveRate;       // %
    double monthlyTDS;
};

// ─── Core calculator class ────────────────────────────────────────────────────
class TaxCalculator
{
public:
    TaxPayerIncome tpiobj;   // public so MainWindow can read/write directly

    TaxCalculator()
    {
        tpiobj = {};            // zero-initialise all numeric fields
        tpiobj.FinancialYear = 2024;
        tpiobj.Age           = 30;
    }

    // ── HRA exemption (call whenever rent / HRA inputs change) ────────────────
    double CalcHRAExemption() const
    {
        if (tpiobj.MonthlyRentPaid <= 0) return 0.0;
        double annualRent = tpiobj.MonthlyRentPaid * 12.0;
        double pct        = tpiobj.IsMetroCity ? 0.50 : 0.40;
        double a = tpiobj.HRA;
        double b = pct * tpiobj.Basic;
        double c = annualRent - 0.10 * tpiobj.Basic;
        double ex = minD(a, minD(b, c));
        return ex < 0 ? 0.0 : ex;
    }

    // ── Full calculation for one regime ──────────────────────────────────────
    TaxResult Compute(bool isNewRegime) const
    {
        TaxResult r = {};

        const double STD_DED = isNewRegime ? 75000.0 : 50000.0;

        // --- Salary ---
        double grossSal = totalSalary();
        double netSal;
        if (isNewRegime)
        {
            netSal = maxD(grossSal - minD(STD_DED, grossSal), 0.0);
        }
        else
        {
            netSal = grossSal - minD(STD_DED, grossSal)
                   - tpiobj.HRAExemption - tpiobj.LTAExemption;
            netSal = maxD(netSal, 0.0);
        }

        // --- House property ---
        double houseProp = 0.0;
        if (tpiobj.GrossRental > 0)
        {
            double nav   = tpiobj.GrossRental - tpiobj.MunicipalTax;
            double stdHP = 0.30 * nav;
            double hlInt = isNewRegime ? 0.0 : minD(tpiobj.HomeLoanInt, 200000.0);
            houseProp    = nav - stdHP - hlInt;
        }
        else if (!isNewRegime)
        {
            houseProp = -minD(tpiobj.HomeLoanInt, 200000.0);
        }

        // --- Other income ---
        double otherInc = totalOtherIncome();

        // --- Normal taxable (before capital gains) ---
        double chapVI = isNewRegime
            ? totalNewRegimeDeductions()
            : (totalOldRegimeDeductions() - tpiobj.HRAExemption - tpiobj.LTAExemption);
        // HRA & LTA were already removed from netSal; chapVI removes remaining deductions

        double normalTaxable = maxD(netSal + houseProp + otherInc - chapVI, 0.0);

        // --- Populate result ---
        r.grossIncome       = grossSal + houseProp + tpiobj.STCG + tpiobj.LTCG + otherInc;
        r.standardDeduction = minD(STD_DED, grossSal);
        r.totalDeductions   = isNewRegime ? totalNewRegimeDeductions()
                                          : totalOldRegimeDeductions();
        r.normalTaxable     = normalTaxable;
        r.taxableIncome     = normalTaxable + tpiobj.STCG + tpiobj.LTCG;

        // --- Slab tax ---
        r.slabTax  = isNewRegime ? newSlabTax(normalTaxable)
                                 : oldSlabTax(normalTaxable, tpiobj.Age);
        r.stcgTax  = stcgTax(tpiobj.STCG);
        r.ltcgTax  = ltcgTax(tpiobj.LTCG);
        r.taxBeforeRebate = r.slabTax + r.stcgTax + r.ltcgTax;

        // --- Rebate 87A ---
        r.rebate87A     = rebate87A(normalTaxable, r.slabTax, isNewRegime);
        r.taxAfterRebate= maxD(r.taxBeforeRebate - r.rebate87A, 0.0);

        // --- Surcharge & Cess ---
        r.surcharge = surcharge(r.taxAfterRebate, r.taxableIncome, isNewRegime);
        r.cess      = (r.taxAfterRebate + r.surcharge) * 0.04;
        r.totalTax  = r.taxAfterRebate + r.surcharge + r.cess;

        r.effectiveRate = (r.grossIncome > 0) ? (r.totalTax / r.grossIncome) * 100.0 : 0.0;
        r.monthlyTDS    = r.totalTax / 12.0;

        return r;
    }

    double CalculateOldRegimeTax() { return Compute(false).totalTax; }
    double CalculateNewRegimeTax() { return Compute(true).totalTax;  }

private:
    static double minD(double a, double b) { return a < b ? a : b; }
    static double maxD(double a, double b) { return a > b ? a : b; }

    double totalSalary() const
    {
        return tpiobj.Basic + tpiobj.HRA + tpiobj.SpecialAllowance
             + tpiobj.Bonus + tpiobj.OtherSalary;
    }

    double totalOtherIncome() const
    {
        return tpiobj.InterestIncome + tpiobj.BusinessIncome + tpiobj.OtherIncome;
    }

    double totalOldRegimeDeductions() const
    {
        double d = 0;
        d += minD(tpiobj.Sec80C,      150000.0);
        d += tpiobj.Sec80D;
        d += tpiobj.Sec80E;
        d += tpiobj.Sec80G;
        d += minD(tpiobj.Sec80TTA,    10000.0);
        d += minD(tpiobj.Sec80CCD1B,  50000.0);
        d += minD(tpiobj.HomeLoanInt, 200000.0);
        d += tpiobj.HRAExemption;
        d += tpiobj.LTAExemption;
        d += tpiobj.OtherDeductions;
        return d;
    }

    double totalNewRegimeDeductions() const
    {
        return tpiobj.EmployerNPS + tpiobj.AgniVeer;
    }

    static double surcharge(double tax, double taxable, bool isNew)
    {
        double rate = 0;
        if (taxable >  5000000  && taxable <= 10000000) rate = 0.10;
        if (taxable > 10000000  && taxable <= 20000000) rate = 0.15;
        if (taxable > 20000000  && taxable <= 50000000) rate = 0.25;
        if (taxable > 50000000) rate = isNew ? 0.25 : 0.37;
        return tax * rate;
    }

    static double rebate87A(double taxable, double tax, bool isNew)
    {
        if (!isNew && taxable <= 500000) return minD(tax, 12500.0);
        if ( isNew && taxable <= 700000) return minD(tax, 25000.0);
        return 0.0;
    }

    static double stcgTax(double stcg) { return stcg > 0 ? stcg * 0.15 : 0.0; }
    static double ltcgTax(double ltcg)
    {
        if (ltcg <= 100000) return 0.0;
        return (ltcg - 100000.0) * 0.10;
    }

    static double oldSlabTax(double taxable, int age)
    {
        double exempt = 250000;
        if (age >= 60 && age < 80) exempt = 300000;
        if (age >= 80)             exempt = 500000;
        if (taxable <= exempt) return 0.0;

        double tax = 0, t;
        t = minD(taxable, 500000.0)  - exempt;    if (t > 0) tax += t * 0.05;
        t = minD(taxable, 1000000.0) - 500000.0;  if (t > 0) tax += t * 0.20;
        t = taxable - 1000000.0;                  if (t > 0) tax += t * 0.30;
        return tax;
    }

    static double newSlabTax(double taxable)
    {
        if (taxable <= 300000) return 0.0;
        double tax = 0, t;
        t = minD(taxable, 700000.0)  - 300000.0;  if (t > 0) tax += t * 0.05;
        t = minD(taxable, 1000000.0) - 700000.0;  if (t > 0) tax += t * 0.10;
        t = minD(taxable, 1200000.0) - 1000000.0; if (t > 0) tax += t * 0.15;
        t = minD(taxable, 1500000.0) - 1200000.0; if (t > 0) tax += t * 0.20;
        t = taxable - 1500000.0;                  if (t > 0) tax += t * 0.30;
        return tax;
    }
};
