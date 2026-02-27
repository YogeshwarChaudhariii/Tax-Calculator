#include <iostream>
#include <string>
#include <algorithm>    // for min()
using namespace std;

//////////////////////////////////////////////////////////////
//
// struct TaxPayerInfo
// Description : Holds all taxpayer data
//
//////////////////////////////////////////////////////////////
struct TaxPayerInfo
{
    // Tax Payer Personal Info
    string TaxPayerName;
    int    Age;

    // Income
    double BasicSalary;         
    double HRAReceived;         
    double SpecialAllowance;    
    double Bonus;
    double RentalIncome;        
    double STCG;                
    double LTCG;                
    double OtherIncome;         

    // HRA details (for old regime HRA exemption)
    double MonthlyRentPaid;
    bool   IsMetroCity;

    // Deductions (Old Regime)
    double Sec80C;              // PF, PPF, ELSS, LIC  
    double Sec80D;              // Health insurance
    double Sec80E;              // Education loan interest
    double HomeLoanInterest;    // Section 24b          
    double OtherDeductions;

    // Deductions (New Regime)
    double EmployerNPS;         
};

//////////////////////////////////////////////////////////////
//
// class TaxCalculator
// Description : Calculate Tax
//
//////////////////////////////////////////////////////////////
class TaxCalculator
{
    private:

    TaxPayerInfo tp;

//////////////////////////////////////////////////////////////
//
// function GrossSalary
// Description : Total gross salary
//
//////////////////////////////////////////////////////////////
    double GrossSalary()
    {
        return tp.BasicSalary + 
               tp.HRAReceived + 
               tp.SpecialAllowance + 
               tp.Bonus;
    }

//////////////////////////////////////////////////////////////
//
// function HRAExemption
// Description : HRA exemption (old regime)
//
//////////////////////////////////////////////////////////////
    double HRAExemption()
    {
        double annualRent = 0, cityPct = 0;
        double iNo1 = 0, iNo2 = 0, iNo3 = 0;   

        if (tp.MonthlyRentPaid <= 0)
        {
            return 0;
        }

        annualRent = tp.MonthlyRentPaid * 12;
        cityPct    = tp.IsMetroCity ? 0.50 : 0.40;

//////////////////////////////////////////////////////////////
//
// Minimum of:
//   iNo1) HRA actually received
//   iNo2) 50% of basic (metro) / 40% of bas
//   iNo3) Annual rent paid - 10% of basic
//
//////////////////////////////////////////////////////////////
        iNo1 = tp.HRAReceived;
        iNo2 = cityPct * tp.BasicSalary;
        iNo3 = annualRent - 0.10 * tp.BasicSalary;

        double exemption = min({ iNo1, iNo2, iNo3 });

        return exemption < 0 ? 0 : exemption;
    }

//////////////////////////////////////////////////////////////
//
// function OldRegimeSlabTax
// Description : Old regime slab tax on normal income
//
//////////////////////////////////////////////////////////////
    double OldRegimeSlabTax(double taxable)
    {
        // Basic exemption depends on age
        double exempt = 250000;
        double tax = 0;

        if (tp.Age >= 60 && tp.Age < 80)
        {
            exempt = 300000;
        }

        if (tp.Age >= 80)       
        {
            exempt = 500000;
        }

        if (taxable <= exempt)
        {
            return 0;
        }
        
        tax += (min(taxable, 500000.0)  - exempt)   * 0.05;   // 5%  slab

        if (taxable > 500000)
        {
            tax += (min(taxable, 1000000.0) - 500000.0) * 0.20;  // 20% slab
        }

        if (taxable > 1000000)
        {
            tax += (taxable - 1000000.0) * 0.30;               // 30% slab
        }

        return tax;
    }
//////////////////////////////////////////////////////////////
//
// function NewRegimeSlabTax
// Description : New regime slab tax 
//
//////////////////////////////////////////////////////////////
    double NewRegimeSlabTax(double taxable)
    {
        double tax = 0;

        if (taxable <= 300000)
        {
            return 0;
        }

        tax += (min(taxable, 700000.0)  - 300000.0) * 0.05;   //  5%

        if (taxable > 700000)
        {
            tax += (min(taxable, 1000000.0) - 700000.0)  * 0.10;  // 10%
        }

        if (taxable > 1000000)
        {
            tax += (min(taxable, 1200000.0) - 1000000.0) * 0.15;  // 15%
        }

        if (taxable > 1200000)
        {
            tax += (min(taxable, 1500000.0) - 1200000.0) * 0.20;  // 20%
        }

        if (taxable > 1500000)
        {
            tax += (taxable - 1500000.0) * 0.30;                   // 30%
        }

        return tax;
    }

//////////////////////////////////////////////////////////////
//
// function CapitalGainsTax
// Description : Capital gains tax
//
//////////////////////////////////////////////////////////////
    double CapitalGainsTax()
    {
        double stcgTax = 0, ltcgTax = 0;

        stcgTax = (tp.STCG > 0) ? tp.STCG * 0.15 : 0;
        ltcgTax = (tp.LTCG > 100000) ? (tp.LTCG - 100000) * 0.10 : 0;

        return stcgTax + ltcgTax;
    }
//////////////////////////////////////////////////////////////
//
// function Surcharge
// Description : Surcharge on tax
//
//////////////////////////////////////////////////////////////
    double Surcharge(double tax, double totalTaxableIncome, bool isNewRegime)
    {
        double rate = 0;
        if (totalTaxableIncome > 5000000  && 
            totalTaxableIncome <= 10000000)
        {
            rate = 0.10;
        }

        if (totalTaxableIncome > 10000000 && 
            totalTaxableIncome <= 20000000)
        {
            rate = 0.15;
        }

        if (totalTaxableIncome > 20000000 && 
            totalTaxableIncome <= 50000000)
        {
            rate = 0.25;
        }

        if (totalTaxableIncome > 50000000) 
        {
            rate = isNewRegime ? 0.25 : 0.37;
        }

        return tax * rate;
    }

//////////////////////////////////////////////////////////////
//
// function Rebate87A
// Description : Rebate u/s 87A
//
//////////////////////////////////////////////////////////////
    double Rebate87A(double normalTaxable, double slabTax, bool isNewRegime)
    {
        if (!isNewRegime && normalTaxable <= 500000)
        {
            return min(slabTax, 12500.0);
        }

        if ( isNewRegime && normalTaxable <= 700000)
        {
            return min(slabTax, 25000.0);
        }

        return 0;
    }

    public:

//////////////////////////////////////////////////////////////
//
// function Accept
// Description : Take all inputs from user
//
//////////////////////////////////////////////////////////////
        void Accept()
        {
            int city = 0;

            cout << "\n********** TAXPAYER DETAILS **********"<< endl;

            cout << "Enter Name: ";
            getline(cin, tp.TaxPayerName);

            cout << "Enter Age: ";
            cin >> tp.Age;

            cout << "\n********** SALARY INCOME **********"<< endl;

            cout << "Basic Salary (Annual): ";
            cin >> tp.BasicSalary;
            cout << "HRA Received (Annual): ";
            cin >> tp.HRAReceived;
            cout << "Special Allowance (Annual): ";
            cin >> tp.SpecialAllowance;
            cout << "Bonus (Annual): ";
            cin >> tp.Bonus;

            cout << "\n********** HRA DETAILS (enter 0 if not applicable) **********" << endl;

            cout << "Monthly Rent Paid: ";
            cin >> tp.MonthlyRentPaid;

            if (tp.MonthlyRentPaid > 0)
            {
                cout << "City Type (1 = Metro, 2 = Non-Metro): ";
                cin >> city;

                tp.IsMetroCity = (city == 1);
            }
            else
            {
                tp.IsMetroCity = false;
            }

            cout << "\n********** OTHER INCOME **********" << endl;

            cout << "Rental Income (from house property): ";
            cin >> tp.RentalIncome;
            cout << "Short-Term Capital Gains (STCG): ";
            cin >> tp.STCG;
            cout << "Long-Term Capital Gains (LTCG): ";
            cin >> tp.LTCG;
            cout << "Other Income (FD interest, etc.): ";
            cin >> tp.OtherIncome;

            cout << "\n********** DEDUCTIONS - OLD REGIME (enter 0 if not applicable) **********" << endl;

            cout << "Sec 80C - PF/PPF/ELSS/LIC (max 1,50,000): ";
            cin >> tp.Sec80C;
            cout << "Sec 80D - Health Insurance Premium: ";
            cin >> tp.Sec80D;
            cout << "Sec 80E - Education Loan Interest: ";
            cin >> tp.Sec80E;
            cout << "Home Loan Interest - Sec 24b (max 2,00,000): ";
            cin >> tp.HomeLoanInterest;
            cout << "Other Deductions: ";
            cin >> tp.OtherDeductions;

            cout << "\n********** DEDUCTIONS - NEW REGIME **********" << endl;

            cout << "(Note: Most deductions are NOT allowed in New Regime)" << endl;
            cout << "Employer NPS Contribution - Sec 80CCD(2): ";
            cin >> tp.EmployerNPS;
        }

//////////////////////////////////////////////////////////////
//
// function CalculateOldRegimeTax
// Description : Returns final tax under old regime
//
//////////////////////////////////////////////////////////////
        double CalculateOldRegimeTax()
        {
            double netSalary = 0, ded80C = 0, dedHL = 0, totalDed = 0, normalTaxable = 0;
            double slabTax = 0, capTax = 0, rebate = 0, taxAfter = 0;
            double totalTaxable = 0, surcharge = 0, cess = 0;

            // Net salary after standard deduction and HRA/LTA
            netSalary = GrossSalary() - 50000 - HRAExemption();

            if (netSalary < 0)
            {
                netSalary = 0;
            }

            // Old regime caps on deductions
            ded80C = min(tp.Sec80C, 150000.0);
            dedHL  = min(tp.HomeLoanInterest, 200000.0);

            // Total old regime deductions
            totalDed = ded80C + 
                       tp.Sec80D + 
                       tp.Sec80E + 
                       dedHL + 
                       tp.OtherDeductions;

            // Normal taxable income (excluding capital gains)
            normalTaxable = netSalary + 
                            tp.RentalIncome + 
                            tp.OtherIncome - 
                            totalDed;

            if (normalTaxable < 0)
            {
                normalTaxable = 0;
            }

            // Slab tax + capital gains tax
            slabTax  = OldRegimeSlabTax(normalTaxable);
            capTax   = CapitalGainsTax();

            // Rebate 87A (only on slab income, not cap gains)
            rebate   = Rebate87A(normalTaxable, slabTax, false);
            taxAfter = (slabTax - rebate) + capTax;

            if (taxAfter < 0) 
            {
                taxAfter = 0;
            }

            // Surcharge + 4% cess
            totalTaxable = normalTaxable + tp.STCG + tp.LTCG;
            surcharge    = Surcharge(taxAfter, totalTaxable, false);
            cess         = (taxAfter + surcharge) * 0.04;

            return taxAfter + surcharge + cess;
        }

//////////////////////////////////////////////////////////////
//
// function CalculateNewRegimeTax
// Description : Returns final tax under new regime
//
//////////////////////////////////////////////////////////////
        double CalculateNewRegimeTax()
        {
            double netSalary = 0, normalTaxable = 0, slabTax = 0, capTax = 0;
            double rebate = 0, taxAfter = 0, totalTaxable = 0, surcharge = 0, cess = 0;

            // Net salary after standard deduction (Rs. 75,000 in new regime)
            netSalary = GrossSalary() - 75000;

            if (netSalary < 0)
            { 
                netSalary = 0;
            }

            // Only employer NPS deduction allowed
            normalTaxable = netSalary + 
                            tp.RentalIncome + 
                            tp.OtherIncome - 
                            tp.EmployerNPS;

            if (normalTaxable < 0) 
            {
                normalTaxable = 0;
            }

            // Slab tax + capital gains tax
            slabTax  = NewRegimeSlabTax(normalTaxable);
            capTax   = CapitalGainsTax();

            // Rebate 87A
            rebate   = Rebate87A(normalTaxable, slabTax, true);
            taxAfter = (slabTax - rebate) + capTax;

            if (taxAfter < 0) 
            {
                taxAfter = 0;
            }

            // Surcharge + 4% cess
            totalTaxable = normalTaxable + tp.STCG + tp.LTCG;
            surcharge    = Surcharge(taxAfter, totalTaxable, true);
            cess         = (taxAfter + surcharge) * 0.04;

            return taxAfter + surcharge + cess;
        }

//////////////////////////////////////////////////////////////
//
// function DisplayMenu
// Description : Show menu and print results
//
//////////////////////////////////////////////////////////////
        void DisplayMenu()
        {
            double oldTax = 0, newTax = 0, saving = 0;
            int choice = 0;

            cout << "\n********** SELECT OPTION **********" << endl;

            cout << "1. Old Regime" << endl;
            cout << "2. New Regime" << endl;
            cout << "3. Compare Both Regimes" << endl;
            cout << "Enter Choice: ";
            cin >> choice;

            oldTax = CalculateOldRegimeTax();
            newTax = CalculateNewRegimeTax();

            cout << "\n--------------------------------------" << endl;
            cout << "Taxpayer : " << tp.TaxPayerName << "  |  Age : " << tp.Age << endl;
            cout << "--------------------------------------" << endl;

            if (choice == 1)
            {
                cout << "Total Tax - Old Regime : Rs. " << oldTax << endl;
            }
            else if (choice == 2)
            {
                cout << "Total Tax - New Regime : Rs. " << newTax << endl;
            }
            else if (choice == 3)
            {
                cout << "Old Regime Tax : Rs. " << oldTax << endl;
                cout << "New Regime Tax : Rs. " << newTax << endl;

                saving = oldTax - newTax;

                if (saving > 0)
                {
                    cout << "\nNew Regime is better! You save Rs. " << saving << " per year." << endl;
                }
                else if (saving < 0)
                {
                    cout << "\nOld Regime is better! You save Rs. " << -saving << " per year." << endl;
                }
                else
                {
                    cout << "\nBoth regimes give the same tax." << endl;
                }
            }
            else
            {
                cout << "Invalid Choice!" << endl;
            }

            cout << "--------------------------------------" << endl;
    }
};

//////////////////////////////////////////////////////////////
//
// function main
// Description : Entry point function
//
//////////////////////////////////////////////////////////////
int main()
{
    TaxCalculator tc;

    tc.Accept();
    tc.DisplayMenu();

    return 0;
}
