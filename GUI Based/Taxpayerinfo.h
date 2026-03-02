#ifndef TAXPAYERINFO_H
#define TAXPAYERINFO_H

#include <QString>

struct TaxPayerInfo
{
    // Personal
    QString TaxPayerName;
    int     Age;

    // Salary Income
    double BasicSalary;
    double HRAReceived;
    double SpecialAllowance;
    double Bonus;

    // HRA Details
    double MonthlyRentPaid;
    bool   IsMetroCity;

    // Other Income
    double RentalIncome;
    double STCG;
    double LTCG;
    double OtherIncome;

    // Old Regime Deductions
    double Sec80C;           // max 1,50,000
    double Sec80D;
    double Sec80E;
    double HomeLoanInterest; // max 2,00,000
    double OtherDeductions;

    // New Regime Deductions
    double EmployerNPS;

    TaxPayerInfo()
        : Age(0)
        , BasicSalary(0), HRAReceived(0), SpecialAllowance(0), Bonus(0)
        , MonthlyRentPaid(0), IsMetroCity(false)
        , RentalIncome(0), STCG(0), LTCG(0), OtherIncome(0)
        , Sec80C(0), Sec80D(0), Sec80E(0), HomeLoanInterest(0), OtherDeductions(0)
        , EmployerNPS(0)
    {
    }
};

#endif // TAXPAYERINFO_H