#include "pch.h"
#include <QtWidgets>
#include <QLocale>

#include "mainwindow.h"
#include "taxcalculator.h"

// ─── Helper: create a rupee-aware spin box ────────────────────────────────────

static QDoubleSpinBox* makeRupeeSpin(double max = 99999999)
{
    auto* sb = new QDoubleSpinBox;
    sb->setPrefix("Rs. ");
    sb->setRange(0, max);
    sb->setDecimals(0);
    sb->setSingleStep(1000);
    sb->setGroupSeparatorShown(true);
    sb->setMinimumWidth(180);
    return sb;
}

// ─── Constructor ──────────────────────────────────────────────────────────────

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Marvellous Tax Calculator — FY 2024-25");
    setMinimumSize(780, 680);

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(buildInputPage(), "Input Details");
    m_tabs->addTab(buildResultPage(), "Tax Results");
    m_tabs->addTab(AboutUs(), "About Us");
    m_tabs->addTab(HelpPage(), "Help");
    setCentralWidget(m_tabs);
}

// ─── Input Page ───────────────────────────────────────────────────────────────

QWidget* MainWindow::buildInputPage()
{
    auto* page = new QWidget;
    auto* vlay = new QVBoxLayout(page);

    // Scroll area so small screens can still access everything
    auto* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    auto* inner = new QWidget;
    auto* ilay = new QVBoxLayout(inner);
    ilay->setSpacing(12);

    ilay->addWidget(buildPersonalGroup());
    ilay->addWidget(buildSalaryGroup());
    ilay->addWidget(buildHRAGroup());
    ilay->addWidget(buildOtherIncomeGroup());
    ilay->addWidget(buildOldDeductionsGroup());
    ilay->addWidget(buildNewDeductionsGroup());

    // ── Regime selection ──
    auto* regimeBox = new QGroupBox("Select Tax Regime to Display");
    auto* rlay = new QHBoxLayout(regimeBox);
    m_rbOld = new QRadioButton("Old Regime");
    m_rbNew = new QRadioButton("New Regime");
    m_rbBoth = new QRadioButton("Compare Both");
    m_rbBoth->setChecked(true);
    rlay->addWidget(m_rbOld);
    rlay->addWidget(m_rbNew);
    rlay->addWidget(m_rbBoth);
    rlay->addStretch();
    ilay->addWidget(regimeBox);

    // ── Buttons ──
    auto* btnRow = new QHBoxLayout;
    m_calcBtn = new QPushButton("Calculate Tax");
    m_calcBtn->setFixedHeight(40);
    m_calcBtn->setStyleSheet(
        "QPushButton { background:#1976D2; color:white; font-weight:bold; "
        "border-radius:5px; font-size:14px; }"
        "QPushButton:hover { background:#1565C0; }"
        "QPushButton:pressed { background:#0D47A1; }");

    auto* resetBtn = new QPushButton("Reset");
    resetBtn->setFixedHeight(40);
    resetBtn->setStyleSheet(
        "QPushButton { background:#607D8B; color:white; font-weight:bold; "
        "border-radius:5px; font-size:14px; }"
        "QPushButton:hover { background:#546E7A; }");

    btnRow->addStretch();
    btnRow->addWidget(resetBtn);
    btnRow->addWidget(m_calcBtn);
    ilay->addLayout(btnRow);
    ilay->addStretch();

    scroll->setWidget(inner);
    vlay->addWidget(scroll);

    connect(m_calcBtn, &QPushButton::clicked, this, &MainWindow::onCalculate);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onReset);

    return page;
}

// ─── Group builders ───────────────────────────────────────────────────────────

QGroupBox* MainWindow::buildPersonalGroup()
{
    auto* grp = new QGroupBox("Taxpayer Details");
    auto* form = new QFormLayout(grp);

    m_name = new QLineEdit;
    m_name->setPlaceholderText("Full name");
    m_age = new QSpinBox;
    m_age->setRange(1, 120);
    m_age->setValue(30);
    m_age->setSuffix(" yrs");

    form->addRow("Name:", m_name);
    form->addRow("Age:", m_age);
    return grp;
}

QGroupBox* MainWindow::buildSalaryGroup()
{
    auto* grp = new QGroupBox("Salary Income (Annual)");
    auto* form = new QFormLayout(grp);

    m_basic = makeRupeeSpin();
    m_hra = makeRupeeSpin();
    m_special = makeRupeeSpin();
    m_bonus = makeRupeeSpin();

    form->addRow("Basic Salary:", m_basic);
    form->addRow("HRA Received:", m_hra);
    form->addRow("Special Allowance:", m_special);
    form->addRow("Bonus:", m_bonus);
    return grp;
}

QGroupBox* MainWindow::buildHRAGroup()
{
    auto* grp = new QGroupBox("HRA Details  (enter 0 if not renting)");
    auto* form = new QFormLayout(grp);

    m_rent = makeRupeeSpin(500000);
    m_metro = new QCheckBox("Metro city  (Pune / Mumbai / Delhi / Kolkata / Chennai)");

    form->addRow("Monthly Rent Paid:", m_rent);
    form->addRow("", m_metro);

    // Disable metro checkbox when rent is 0
    connect(m_rent, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &MainWindow::onRentToggled);
    onRentToggled(0);

    return grp;
}

QGroupBox* MainWindow::buildOtherIncomeGroup()
{
    auto* grp = new QGroupBox("Other Income");
    auto* form = new QFormLayout(grp);

    m_rentalIncome = makeRupeeSpin();
    m_stcg = makeRupeeSpin();
    m_ltcg = makeRupeeSpin();
    m_otherIncome = makeRupeeSpin();

    form->addRow("Rental Income (House Property):", m_rentalIncome);
    form->addRow("Short-Term Capital Gains (STCG):", m_stcg);
    form->addRow("Long-Term Capital Gains (LTCG):", m_ltcg);
    form->addRow("Other Income (FD interest etc.):", m_otherIncome);
    return grp;
}

QGroupBox* MainWindow::buildOldDeductionsGroup()
{
    auto* grp = new QGroupBox("Deductions — Old Regime");
    auto* form = new QFormLayout(grp);

    m_sec80C = makeRupeeSpin(150000);
    m_sec80D = makeRupeeSpin();
    m_sec80E = makeRupeeSpin();
    m_homeLoan = makeRupeeSpin(200000);
    m_otherDed = makeRupeeSpin();

    form->addRow("Sec 80C — PF / PPF / ELSS / LIC  (max ₹1,50,000):", m_sec80C);
    form->addRow("Sec 80D — Health Insurance Premium:", m_sec80D);
    form->addRow("Sec 80E — Education Loan Interest:", m_sec80E);
    form->addRow("Home Loan Interest Sec 24b  (max ₹2,00,000):", m_homeLoan);
    form->addRow("Other Deductions:", m_otherDed);
    return grp;
}

QGroupBox* MainWindow::buildNewDeductionsGroup()
{
    auto* grp = new QGroupBox("Deductions — New Regime");
    auto* form = new QFormLayout(grp);

    auto* note = new QLabel("Most deductions are <b>not</b> allowed under the new regime.");
    note->setStyleSheet("color:#E65100;");
    form->addRow(note);

    m_nps = makeRupeeSpin();
    form->addRow("Employer NPS Contribution — Sec 80CCD(2):", m_nps);
    return grp;
}

// ─── Result Page ──────────────────────────────────────────────────────────────

QWidget* MainWindow::buildResultPage()
{
    auto* page = new QWidget;
    auto* vlay = new QVBoxLayout(page);

    m_result = new QTextEdit;
    m_result->setReadOnly(true);
    m_result->setFont(QFont("Courier New", 11));
    m_result->setStyleSheet(
        "QTextEdit { background:#1E1E2E; color:#CDD6F4; "
        "border:1px solid #45475A; border-radius:6px; padding:10px; }");
    m_result->setPlaceholderText(
        "Fill in the details on the Input tab and click Calculate Tax to see results here.");

    vlay->addWidget(m_result);
    return page;
}

// ─── About Us ──────────────────────────────────────────────────────────────
QWidget* MainWindow::AboutUs()
{
    auto* page = new QWidget;
    auto* vlay = new QVBoxLayout(page);

    m_about = new QTextEdit;
    m_about->setReadOnly(true);
    m_about->setFont(QFont("Courier New", 11));
    m_about->setStyleSheet(
        "QTextEdit { background:#1E1E2E; color:#CDD6F4; "
        "border:1px solid #45475A; border-radius:6px; padding:10px; }");

    m_about->setHtml(
        "<h2>Marvellous Tax Calculator</h2<br>"
        "I am a student of <b>Marvellous Infosystems</b>. This software was developed at the same institute.<br>" 
        "The project was completed by Yogeshwar Chaudhari under the guidance of Piyush Khairnar Sir."
    );

    vlay->addWidget(m_about);
    return page;
}

// ─── Help Page ──────────────────────────────────────────────────────────────
QWidget* MainWindow::HelpPage()
{
    auto* page = new QWidget;
    auto* vlay = new QVBoxLayout(page);

    m_querys = new QTextEdit;
    m_querys->setReadOnly(true);
    m_querys->setFont(QFont("Courier New", 11));
    m_querys->setStyleSheet(
        "QTextEdit { background:#1E1E2E; color:#CDD6F4; "
        "border:1px solid #45475A; border-radius:6px; padding:10px; }");
    
    m_querys->setText(
        "Income Tax Help Section\n\n"
        "Information About Income Tax: \n\n"
        "1) Section 80C\n"
        "Maximum Limit : Rs. 1.5 Lakh per year\n"
        "Covers Investments and expenses like: \n"
        "PPF (Public Provident Fund)\n"
        "EPF (Employees Provident Fund)\n"
        "LIC (Life Insurance Premium)\n"
        "ELSS Mututal Fund\n"
        "Home Loan Principal Repayment\n"
        "Tutions Fees For Children\n\n"

        "2) Section 80D\n"
        "Deduction For Health Insurance Premium\n"
        "Helps Reduce Tax if you pay medical insurance\n\n"

        "3) HRA (Home Rent Allowance)\n"
        "HRA Exemptions depends on: \n"
        "Rent Paid = 10% of salary\n"
        "40% (Non-Metro) or 50% (Metro) of salary\n"
        "The lowest of the above is allowed as deduction"
    );

    vlay->addWidget(m_querys);
    return page;
}

// ─── Slots ────────────────────────────────────────────────────────────────────

void MainWindow::onRentToggled(double rent)
{
    m_metro->setEnabled(rent > 0);
    if (rent <= 0) m_metro->setChecked(false);
}

void MainWindow::onReset()
{
    m_name->clear();
    m_age->setValue(30);

    for (auto* sb : findChildren<QDoubleSpinBox*>())
        sb->setValue(0);

    m_metro->setChecked(false);
    m_rbBoth->setChecked(true);
    m_result->clear();
    m_tabs->setCurrentIndex(0);
}

void MainWindow::onCalculate()
{
    if (m_name->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing Info", "Please enter the taxpayer's name.");
        return;
    }

    TaxPayerInfo tp = collectData();
    TaxCalculator calc(tp);

    RegimeResult oldR = calc.calculateOldRegime();
    RegimeResult newR = calc.calculateNewRegime();

    int choice = m_rbOld->isChecked() ? 1 : (m_rbNew->isChecked() ? 2 : 3);

    m_result->setHtml(formatResult(tp.TaxPayerName, tp.Age, oldR, newR, choice));
    m_tabs->setCurrentIndex(1);
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

TaxPayerInfo MainWindow::collectData() const
{
    TaxPayerInfo tp;
    tp.TaxPayerName = m_name->text().trimmed();
    tp.Age = m_age->value();
    tp.BasicSalary = m_basic->value();
    tp.HRAReceived = m_hra->value();
    tp.SpecialAllowance = m_special->value();
    tp.Bonus = m_bonus->value();
    tp.MonthlyRentPaid = m_rent->value();
    tp.IsMetroCity = m_metro->isChecked();
    tp.RentalIncome = m_rentalIncome->value();
    tp.STCG = m_stcg->value();
    tp.LTCG = m_ltcg->value();
    tp.OtherIncome = m_otherIncome->value();
    tp.Sec80C = m_sec80C->value();
    tp.Sec80D = m_sec80D->value();
    tp.Sec80E = m_sec80E->value();
    tp.HomeLoanInterest = m_homeLoan->value();
    tp.OtherDeductions = m_otherDed->value();
    tp.EmployerNPS = m_nps->value();
    return tp;
}

static QString inr(double v)
{
    return QLocale(QLocale::English, QLocale::India)
        .toCurrencyString(v, "Rs. ", 0);
}

static QString row(const QString& label, double val, const QString& color = "#CDD6F4")
{
    return QString("<tr><td style='padding:2px 12px 2px 4px; color:#A6ADC8;'>%1</td>"
        "<td style='color:%3; text-align:right;'>%2</td></tr>")
        .arg(label, inr(val), color);
}

QString MainWindow::formatResult(const QString& name, int age,
    const RegimeResult& oldR,
    const RegimeResult& newR,
    int choice) const
{
    QString html;
    html += "<style>body{font-family:'Courier New',monospace;font-size:12px;} "
        "h2{color:#89DCEB;} h3{color:#A6E3A1;} "
        "table{width:100%;border-collapse:collapse;} "
        ".divider{color:#45475A;} .total{color:#F38BA8;font-weight:bold;} "
        ".saving{color:#A6E3A1;font-size:14px;font-weight:bold;}</style>";

    html += QString("<h2>Tax Computation — %1 (Age: %2)</h2>").arg(name).arg(age);

    auto buildBlock = [&](const QString& title, const RegimeResult& r) {
        QString s;
        s += QString("<h3>%1</h3><table>").arg(title);
        s += row("Net Salary (after Std. Deduction & HRA)", r.netSalary);
        s += row("Total Deductions", r.totalDed, "#89B4FA");
        s += row("Normal Taxable Income", r.normalTaxable);
        s += "<tr><td colspan='2'><hr style='border:0.5px solid #45475A;'></td></tr>";
        s += row("Slab Tax", r.slabTax);
        s += row("Capital Gains Tax", r.capTax);
        s += row("Less: Rebate u/s 87A", r.rebate, "#A6E3A1");
        s += row("Surcharge", r.surchargeAmt);
        s += row("Health & Education Cess (4%)", r.cess);
        s += "<tr><td colspan='2'><hr style='border:0.5px solid #45475A;'></td></tr>";
        s += QString("<tr><td style='color:#F38BA8;font-weight:bold;padding:4px;'>Total Tax Payable</td>"
            "<td style='color:#F38BA8;font-weight:bold;text-align:right;'>%1</td></tr>")
            .arg(inr(r.totalTax));
        s += "</table>";
        return s;
        };

    if (choice == 1 || choice == 3)
        html += buildBlock("OLD REGIME", oldR);

    if (choice == 2 || choice == 3)
        html += buildBlock("NEW REGIME", newR);

    if (choice == 3) {
        double saving = oldR.totalTax - newR.totalTax;
        html += "<br>";
        if (saving > 0)
            html += QString("<p class='saving'>New Regime saves you %1 / year</p>").arg(inr(saving));
        else if (saving < 0)
            html += QString("<p class='saving'>Old Regime saves you %1 / year</p>").arg(inr(-saving));
        else
            html += "<p class='saving'>Both regimes result in the same tax.</p>";
    }

    return html;
}