#include<iostream>
#include<string>
using namespace std;

struct TaxPayerIncome
{
    string TaxPayerName;
    int Age;
    double AnnualSalary;                  // Annual CTC / Gross Salary
    double OtherIncome;             // FD, Interest, Rent
    double Deductions;               // 80C, 80D, HRA
};

class TaxCalculator
{
    private:
        TaxPayerIncome tpiobj;

    public:

        void Accept()
        {
            cout<<"Enter Name: "<<endl;
            getline(cin, tpiobj.TaxPayerName);

            cout<<"Enter Age: "<<endl;
            cin>>tpiobj.Age;

            cout<<"Enter Annual Salary: "<<endl;
            cin>>tpiobj.AnnualSalary;

            cout<<"Enter Other Income: "<<endl;
            cin>>tpiobj.OtherIncome;

            cout<<"Enter Deductions (Only for old Regime): "<<endl;
            cin>>tpiobj.Deductions;
        }

        double CalculateOldRegimeTax()
        {
            double Gross = 0.0, Taxable = 0.0, Tax = 0.0;
            double Exempt = 250000;

            Gross = tpiobj.AnnualSalary + tpiobj.OtherIncome;

            Taxable = Gross - tpiobj.Deductions - 50000;        // std deduction

            if (Taxable < 0)
            {
                Taxable = 0;
            }

            if (tpiobj.Age >= 60 && tpiobj.Age < 80)
            {
                Exempt = 300000;
            }
            else if (tpiobj.Age >= 80)
            {
                Exempt = 500000;
            }

            if (Taxable > Exempt)
            {
                Tax += (min(Taxable, 500000.0) - Exempt) * 0.05;
            }

            if (Taxable > 500000)
            {
                Tax += (min(Taxable, 1000000.0) - Exempt) * 0.20;
            }

            if (Taxable > 1000000)
            {
                Tax += ((Taxable, 1000000) - Exempt) * 0.30;
            }

            return Tax + (Tax * 0.04);      // 4% cess
        }

        double CalculateNewRegimeTax()
        {
            double Gross = 0.0, Taxable = 0.0, Tax = 0.0;

            Gross = tpiobj.AnnualSalary + tpiobj.OtherIncome;

            Taxable = Gross - 75000;        // std deduction

            if (Taxable < 0)
            {
                Taxable = 0;
            }

            if (Taxable > 300000)
            {
                Tax += (min(Taxable, 700000.0) - 300000) * 0.05;
            }

            if (Taxable > 700000)
            {
                Tax += (min(Taxable, 1000000.0) - 700000) * 0.10;
            }

            if (Taxable > 1000000)
            {
                Tax += (min(Taxable, 1200000.0) - 1000000) * 0.15;
            }

            if (Taxable > 1200000)
            {
                Tax += (min(Taxable, 1500000.0) - 1200000) * 0.20;
            }

            if (Taxable > 1500000)
            {
                Tax += (Taxable - 1500000) * 0.30;
            }

            return Tax + (Tax * 0.04);          // 4% cess
        }

        void DisplayMenu()
        {
            int iChoice = 0;
            double OldTax = 0.0, NewTax = 0.0;

            cout<<endl<<"1. Old Regime"<<endl
                      <<"2. New Regime"<<endl
                      <<"3. Compare Both"<<endl
                      <<"Enter Choice: "<<endl;
            cin>>iChoice;

            OldTax = CalculateOldRegimeTax();
            NewTax = CalculateNewRegimeTax();

            if (iChoice == 1)
            {
                cout<<endl<<"Total Tax (Old Regime): "<<OldTax<<endl;
            }
            else if (iChoice == 2)
            {
                cout<<"Total Tax (New Regime): "<<NewTax<<endl;
            }
            else if (iChoice == 3)
            {
                cout<<"Old Regime Tax: "<<OldTax<<endl;
                cout<<"New Regime Tax: "<<NewTax<<endl;

                if (OldTax < NewTax)
                {
                    cout<<"Old Regime is better"<<endl;
                }
                else if (NewTax < OldTax)
                {
                    cout<<"New Regime is better"<<endl;
                }
                else
                {
                    cout<<"Both are same"<<endl;
                }
            }
            else
            {
                cout<<"Invalid Choice"<<endl;
            } 
        }
};

int main()
{
    TaxCalculator tobj;

    tobj.Accept();
    tobj.DisplayMenu();


    return 0;

}
