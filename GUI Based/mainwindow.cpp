// mainwindow.cpp
// Qt GUI for the Income Tax Calculator (FY 2024-25)
// Mirrors the p1.cpp logic via TaxCalculator.h

#include "mainwindow.h"

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QFrame>
#include <QFont>
#include <QPalette>
#include <QLocale>
#include <cstring>

// ─── colour constants ─────────────────────────────────────────────────────────
static const QString kBgDark     = "#1e1e2e";
static const QString kBgCard     = "#2a2a3e";
static const QString kBgTab      = "#252535";
static const QString kAccentBlue = "#4f8ef7";
static const QString kAccentGrn  = "#43d97b";
static const QString kAccentRed  = "#f7674f";
static const QString kAccentAmb  = "#f7b84f";
static const QString kTextPri    = "#e8eaf6";
static const QString kTextSec    = "#9fa8da";
static const QString kBorder     = "#3a3a5c";

// ─────────────────────────────────────────────────────────────────────────────
//  helper: create a styled QDoubleSpinBox
// ─────────────────────────────────────────────────────────────────────────────
static QDoubleSpinBox* makeDSB(double max = 100000000.0, QWidget *parent = nullptr)
{
    auto *w = new QDoubleSpinBox(parent);
    w->setRange(0, max);
    w->setDecimals(2);
    w->setSingleStep(1000);
    w->setPrefix("₹ ");
    w->setGroupSeparatorShown(true);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    w->setMinimumHeight(30);
    w->setStyleSheet(
        QString("QDoubleSpinBox {"
                "  background: %1; color: %2; border: 1px solid %3;"
                "  border-radius: 5px; padding: 2px 6px;"
                "}"
                "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {"
                "  background: %4; width: 18px; border-radius: 3px;"
                "}")
        .arg(kBgDark, kTextPri, kBorder, kBgCard));
    return w;
}

static QSpinBox* makeSB(int lo, int hi, QWidget *parent = nullptr)
{
    auto *w = new QSpinBox(parent);
    w->setRange(lo, hi);
    w->setMinimumHeight(30);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    w->setStyleSheet(
        QString("QSpinBox {"
                "  background: %1; color: %2; border: 1px solid %3;"
                "  border-radius: 5px; padding: 2px 6px;"
                "}"
                "QSpinBox::up-button, QSpinBox::down-button {"
                "  background: %4; width: 18px; border-radius: 3px;"
                "}")
        .arg(kBgDark, kTextPri, kBorder, kBgCard));
    return w;
}

static QGroupBox* makeGroup(const QString &title)
{
    auto *g = new QGroupBox(title);
    g->setStyleSheet(
        QString("QGroupBox {"
                "  color: %1; border: 1px solid %2; border-radius: 8px;"
                "  margin-top: 12px; font-weight: bold; font-size: 12px;"
                "}"
                "QGroupBox::title {"
                "  subcontrol-origin: margin; subcontrol-position: top left;"
                "  padding: 0 8px;"
                "}")
        .arg(kTextSec, kBorder));
    return g;
}

static QLabel* makeLabel(const QString &text, bool small = false)
{
    auto *l = new QLabel(text);
    l->setStyleSheet(QString("color: %1; font-size: %2px;")
                     .arg(small ? kTextSec : kTextPri)
                     .arg(small ? 11 : 12));
    l->setWordWrap(true);
    return l;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor / Destructor
// ─────────────────────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Income Tax Calculator – FY 2024-25");
    setMinimumSize(900, 700);
    resize(1050, 780);

    // Application-wide dark style
    qApp->setStyleSheet(
        QString("QMainWindow, QWidget { background: %1; color: %2; font-family: 'Segoe UI', sans-serif; }"
                "QTabWidget::pane { border: 1px solid %3; border-radius: 6px; }"
                "QTabBar::tab { background: %4; color: %5; padding: 8px 18px;"
                "               border-radius: 4px 4px 0 0; margin-right: 2px; }"
                "QTabBar::tab:selected { background: %6; color: white; }"
                "QScrollArea { border: none; }"
                "QScrollBar:vertical { background: %4; width: 8px; border-radius: 4px; }"
                "QScrollBar::handle:vertical { background: %3; border-radius: 4px; }"
                "QLineEdit { background: %7; color: %2; border: 1px solid %3;"
                "            border-radius: 5px; padding: 4px 8px; min-height: 28px; }"
                "QCheckBox { color: %2; }"
                "QCheckBox::indicator { width: 16px; height: 16px; }")
        .arg(kBgDark, kTextPri, kBorder, kBgTab, kTextSec, kAccentBlue, kBgDark));

    buildUI();
}

MainWindow::~MainWindow() {}

// ─────────────────────────────────────────────────────────────────────────────
//  buildUI – root layout
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::buildUI()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(16, 12, 16, 12);
    root->setSpacing(10);

    // ── Header ────────────────────────────────────────────────────────────────
    auto *header = new QLabel("🧾  India Income Tax Calculator – FY 2024-25");
    header->setAlignment(Qt::AlignCenter);
    header->setStyleSheet(
        QString("color: %1; font-size: 20px; font-weight: bold;"
                " background: %2; border-radius: 10px; padding: 12px;")
        .arg(kTextPri, kBgCard));
    root->addWidget(header);

    // ── Tab widget ────────────────────────────────────────────────────────────
    auto *tabs = new QTabWidget;
    tabs->setDocumentMode(true);
    root->addWidget(tabs, 1);

    buildPersonalTab(tabs);
    buildSalaryTab(tabs);
    buildPropertyTab(tabs);
    buildCapitalTab(tabs);
    buildOtherIncomeTab(tabs);
    buildDeductionsOldTab(tabs);
    buildDeductionsNewTab(tabs);
    buildResultsTab(tabs);

    // ── Bottom button bar ─────────────────────────────────────────────────────
    auto *btnRow = new QHBoxLayout;
    btnReset = new QPushButton("🔄  Reset");
    btnCalc  = new QPushButton("⚡  Calculate Tax");
    btnCalc->setDefault(true);

    auto btnStyle = [](const QString &bg, const QString &hover) {
        return QString("QPushButton { background: %1; color: white; font-size: 13px; font-weight: bold;"
                       " border: none; border-radius: 8px; padding: 10px 28px; min-width: 140px; }"
                       "QPushButton:hover { background: %2; }").arg(bg, hover);
    };
    btnReset->setStyleSheet(btnStyle("#555577", "#6666aa"));
    btnCalc->setStyleSheet(btnStyle(kAccentBlue, "#3a7de0"));

    btnRow->addStretch();
    btnRow->addWidget(btnReset);
    btnRow->addSpacing(12);
    btnRow->addWidget(btnCalc);
    root->addLayout(btnRow);

    connect(btnCalc,  &QPushButton::clicked, this, &MainWindow::onCalculate);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tab 1 – Personal Information
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::buildPersonalTab(QTabWidget *tabs)
{
    auto *page  = new QWidget;
    auto *outer = new QVBoxLayout(page);
    outer->setAlignment(Qt::AlignTop);

    auto *grp = makeGroup("Taxpayer Information");
    auto *fl  = new QFormLayout;
    fl->setSpacing(10); fl->setContentsMargins(14, 18, 14, 14);
    fl->setLabelAlignment(Qt::AlignRight);

    edName = new QLineEdit; edName->setPlaceholderText("e.g. Rahul Sharma");
    sbAge  = makeSB(18, 120);    sbAge->setValue(30);
    sbFY   = makeSB(2020, 2035); sbFY->setValue(2024);
    sbFY->setSuffix(" (FY xx–" + QString::number(sbFY->value()+1).right(2) + ")");
    connect(sbFY, QOverload<int>::of(&QSpinBox::valueChanged), [this](int v){
        sbFY->setSuffix(QString(" (FY %1–%2)").arg(v).arg(v+1));
    });

    fl->addRow(makeLabel("Full Name:"),         edName);
    fl->addRow(makeLabel("Age (years):"),       sbAge);
    fl->addRow(makeLabel("Financial Year:"),    sbFY);

    grp->setLayout(fl);
    outer->addWidget(grp);
    outer->addStretch();

    tabs->addTab(page, "👤 Personal");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tab 2 – Salary Income + HRA
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::buildSalaryTab(QTabWidget *tabs)
{
    auto *scroll = new QScrollArea; scroll->setWidgetResizable(true);
    auto *page   = new QWidget;
    auto *outer  = new QVBoxLayout(page);
    outer->setAlignment(Qt::AlignTop);

    // Salary group
    auto *grpSal = makeGroup("Annual Salary Components");
    auto *fl1    = new QFormLayout;
    fl1->setSpacing(10); fl1->setContentsMargins(14,18,14,14);
    fl1->setLabelAlignment(Qt::AlignRight);

    sbBasic   = makeDSB(); sbHRA     = makeDSB();
    sbSpecial = makeDSB(); sbBonus   = makeDSB();
    sbOtherSal= makeDSB();

    fl1->addRow(makeLabel("Basic Salary:"),           sbBasic);
    fl1->addRow(makeLabel("HRA Received:"),           sbHRA);
    fl1->addRow(makeLabel("Special Allowance:"),      sbSpecial);
    fl1->addRow(makeLabel("Bonus:"),                  sbBonus);
    fl1->addRow(makeLabel("Other Salary:"),           sbOtherSal);
    grpSal->setLayout(fl1);
    outer->addWidget(grpSal);

    // HRA Exemption group
    auto *grpHRA = makeGroup("HRA Exemption Details  (leave Rent = 0 if not applicable)");
    auto *fl2    = new QFormLayout;
    fl2->setSpacing(10); fl2->setContentsMargins(14,18,14,14);
    fl2->setLabelAlignment(Qt::AlignRight);

    sbRent   = makeDSB(500000);
    chkMetro = new QCheckBox("Metro city (Mumbai / Delhi / Chennai / Kolkata)");
    chkMetro->setStyleSheet(QString("color: %1;").arg(kTextPri));
    lblHRAExemption = new QLabel("₹ 0.00");
    lblHRAExemption->setStyleSheet(
        QString("color: %1; font-weight: bold; font-size: 13px;").arg(kAccentGrn));

    fl2->addRow(makeLabel("Monthly Rent Paid:"),            sbRent);
    fl2->addRow(makeLabel(""),                              chkMetro);
    fl2->addRow(makeLabel("Auto HRA Exemption (Sec 10):"),  lblHRAExemption);
    grpHRA->setLayout(fl2);
    outer->addWidget(grpHRA);

    outer->addStretch();
    scroll->setWidget(page);
    tabs->addTab(scroll, "💼 Salary");

    connect(sbRent,   QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onRentChanged);
    connect(sbBasic,  QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onRentChanged);
    connect(sbHRA,    QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onRentChanged);
    connect(chkMetro, &QCheckBox::stateChanged,
            this, [this](int){ onRentChanged(sbRent->value()); });
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tab 3 – House Property
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::buildPropertyTab(QTabWidget *tabs)
{
    auto *page  = new QWidget;
    auto *outer = new QVBoxLayout(page);
    outer->setAlignment(Qt::AlignTop);

    auto *grp = makeGroup("House Property Income / Loss");
    auto *fl  = new QFormLayout;
    fl->setSpacing(10); fl->setContentsMargins(14,18,14,14);
    fl->setLabelAlignment(Qt::AlignRight);

    sbRental  = makeDSB(); sbMunTax = makeDSB(); sbHomeLoan = makeDSB();

    fl->addRow(makeLabel("Gross Annual Rental Income:"),   sbRental);
    fl->addRow(makeLabel("Municipal Tax Paid:"),           sbMunTax);
    fl->addRow(makeLabel("Home Loan Interest – Sec 24b\n(max ₹2,00,000 in Old Regime):"), sbHomeLoan);

    auto *note = makeLabel("ℹ  In New Regime, home loan interest deduction is NOT allowed.", true);
    fl->addRow(note);

    grp->setLayout(fl);
    outer->addWidget(grp);
    outer->addStretch();
    tabs->addTab(page, "🏠 Property");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tab 4 – Capital Gains
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::buildCapitalTab(QTabWidget *tabs)
{
    auto *page  = new QWidget;
    auto *outer = new QVBoxLayout(page);
    outer->setAlignment(Qt::AlignTop);

    auto *grp = makeGroup("Capital Gains  (taxed separately from slab)");
    auto *fl  = new QFormLayout;
    fl->setSpacing(10); fl->setContentsMargins(14,18,14,14);
    fl->setLabelAlignment(Qt::AlignRight);

    sbSTCG = makeDSB(); sbLTCG = makeDSB();
    fl->addRow(makeLabel("Short-Term Capital Gains (STCG @15%):"),           sbSTCG);
    fl->addRow(makeLabel("Long-Term Capital Gains  (LTCG @10% above ₹1L):"), sbLTCG);

    grp->setLayout(fl);
    outer->addWidget(grp);
    outer->addStretch();
    tabs->addTab(page, "📈 Cap. Gains");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tab 5 – Other Income
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::buildOtherIncomeTab(QTabWidget *tabs)
{
    auto *page  = new QWidget;
    auto *outer = new QVBoxLayout(page);
    outer->setAlignment(Qt::AlignTop);

    auto *grp = makeGroup("Other Sources of Income");
    auto *fl  = new QFormLayout;
    fl->setSpacing(10); fl->setContentsMargins(14,18,14,14);
    fl->setLabelAlignment(Qt::AlignRight);

    sbInterest = makeDSB(); sbBusiness = makeDSB(); sbOtherInc = makeDSB();
    fl->addRow(makeLabel("Interest Income (FD / Savings):"), sbInterest);
    fl->addRow(makeLabel("Business / Profession Income:"),   sbBusiness);
    fl->addRow(makeLabel("Any Other Income:"),               sbOtherInc);

    grp->setLayout(fl);
    outer->addWidget(grp);
    outer->addStretch();
    tabs->addTab(page, "💰 Other Income");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tab 6 – Deductions Old Regime
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::buildDeductionsOldTab(QTabWidget *tabs)
{
    auto *scroll = new QScrollArea; scroll->setWidgetResizable(true);
    auto *page   = new QWidget;
    auto *outer  = new QVBoxLayout(page);
    outer->setAlignment(Qt::AlignTop);

    auto *grp = makeGroup("Chapter VI-A Deductions  (Old Regime only)");
    auto *fl  = new QFormLayout;
    fl->setSpacing(10); fl->setContentsMargins(14,18,14,14);
    fl->setLabelAlignment(Qt::AlignRight);

    sb80C      = makeDSB(150000);
    sb80D      = makeDSB(); sb80E  = makeDSB();
    sb80G      = makeDSB(); sb80TTA= makeDSB(10000);
    sb80CCD1B  = makeDSB(50000);
    sbLTA      = makeDSB(); sbOtherDed = makeDSB();

    fl->addRow(makeLabel("Sec 80C – PF / PPF / ELSS / LIC  (max ₹1,50,000):"), sb80C);
    fl->addRow(makeLabel("Sec 80D – Health Insurance Premium:"),                sb80D);
    fl->addRow(makeLabel("Sec 80E – Education Loan Interest:"),                 sb80E);
    fl->addRow(makeLabel("Sec 80G – Donations:"),                               sb80G);
    fl->addRow(makeLabel("Sec 80TTA – Savings Interest  (max ₹10,000):"),       sb80TTA);
    fl->addRow(makeLabel("Sec 80CCD(1B) – NPS Extra  (max ₹50,000):"),          sb80CCD1B);
    fl->addRow(makeLabel("LTA Exemption:"),                                     sbLTA);
    fl->addRow(makeLabel("Other Deductions:"),                                  sbOtherDed);

    grp->setLayout(fl);
    outer->addWidget(grp);
    outer->addStretch();
    scroll->setWidget(page);
    tabs->addTab(scroll, "📋 Ded. (Old)");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tab 7 – Deductions New Regime
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::buildDeductionsNewTab(QTabWidget *tabs)
{
    auto *page  = new QWidget;
    auto *outer = new QVBoxLayout(page);
    outer->setAlignment(Qt::AlignTop);

    auto *note = makeLabel(
        "⚠  Most deductions are NOT available under the New Regime.\n"
        "   Only Employer NPS (Sec 80CCD(2)) and Agniveer Corpus are allowed.", true);
    note->setStyleSheet(QString("color: %1; background: #3a2a1e;"
                                " border: 1px solid %2; border-radius: 6px;"
                                " padding: 10px; font-size: 12px;").arg(kAccentAmb, kAccentAmb));
    outer->addWidget(note);

    auto *grp = makeGroup("Allowed Deductions – New Regime");
    auto *fl  = new QFormLayout;
    fl->setSpacing(10); fl->setContentsMargins(14,18,14,14);
    fl->setLabelAlignment(Qt::AlignRight);

    sbEmpNPS  = makeDSB(); sbAgniVeer = makeDSB();
    fl->addRow(makeLabel("Employer NPS Contribution – Sec 80CCD(2):"), sbEmpNPS);
    fl->addRow(makeLabel("Agniveer Corpus Fund:"),                      sbAgniVeer);

    grp->setLayout(fl);
    outer->addWidget(grp);
    outer->addStretch();
    tabs->addTab(page, "📋 Ded. (New)");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Tab 8 – Results
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::buildResultsTab(QTabWidget *tabs)
{
    auto *page  = new QWidget;
    auto *outer = new QVBoxLayout(page);
    outer->setContentsMargins(12,12,12,12);
    outer->setSpacing(10);

    // Summary banner
    auto *banner = new QWidget;
    banner->setStyleSheet(QString("background: %1; border-radius: 10px;").arg(kBgCard));
    auto *bannerRow = new QHBoxLayout(banner);
    bannerRow->setContentsMargins(16,14,16,14);

    auto mkBox = [](const QString &title, QLabel **out, const QString &color) {
        auto *col = new QWidget;
        auto *vl  = new QVBoxLayout(col);
        vl->setSpacing(4);
        auto *t = new QLabel(title);
        t->setStyleSheet(QString("color: #9fa8da; font-size: 11px; font-weight: bold;"));
        t->setAlignment(Qt::AlignCenter);
        *out = new QLabel("₹ 0.00");
        (*out)->setAlignment(Qt::AlignCenter);
        (*out)->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: bold;").arg(color));
        vl->addWidget(t);
        vl->addWidget(*out);
        return col;
    };

    bannerRow->addWidget(mkBox("OLD REGIME TAX",  &lblOldTax,    kAccentAmb));

    auto *sep = new QFrame; sep->setFrameShape(QFrame::VLine);
    sep->setStyleSheet(QString("color: %1;").arg(kBorder));
    bannerRow->addWidget(sep);

    bannerRow->addWidget(mkBox("NEW REGIME TAX",  &lblNewTax,    kAccentBlue));

    auto *sep2 = new QFrame; sep2->setFrameShape(QFrame::VLine);
    sep2->setStyleSheet(QString("color: %1;").arg(kBorder));
    bannerRow->addWidget(sep2);

    lblRecommend = new QLabel("–");
    lblRecommend->setAlignment(Qt::AlignCenter);
    lblRecommend->setWordWrap(true);
    lblRecommend->setStyleSheet(QString("color: %1; font-size: 15px; font-weight: bold;")
                                .arg(kAccentGrn));
    auto *recCol = new QWidget;
    auto *recVL  = new QVBoxLayout(recCol);
    auto *recTitle = new QLabel("RECOMMENDATION");
    recTitle->setStyleSheet("color: #9fa8da; font-size: 11px; font-weight: bold;");
    recTitle->setAlignment(Qt::AlignCenter);
    recVL->addWidget(recTitle);
    recVL->addWidget(lblRecommend);
    bannerRow->addWidget(recCol);

    outer->addWidget(banner);

    // Detailed breakdown text area
    txtResults = new QTextEdit;
    txtResults->setReadOnly(true);
    txtResults->setStyleSheet(
        QString("QTextEdit { background: %1; color: %2; border: 1px solid %3;"
                " border-radius: 8px; font-family: 'Courier New', monospace; font-size: 12px; }")
        .arg(kBgDark, kTextPri, kBorder));
    txtResults->setPlaceholderText("Fill in your details and click  ⚡ Calculate Tax  to see the breakdown here.");
    outer->addWidget(txtResults, 1);

    tabs->addTab(page, "📊 Results");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Slot: live HRA exemption update
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onRentChanged(double)
{
    calc.tpiobj.Basic          = sbBasic->value();
    calc.tpiobj.HRA            = sbHRA->value();
    calc.tpiobj.MonthlyRentPaid= sbRent->value();
    calc.tpiobj.IsMetroCity    = chkMetro->isChecked();

    double ex = calc.CalcHRAExemption();
    lblHRAExemption->setText(formatMoney(ex));
    calc.tpiobj.HRAExemption = ex;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Slot: Calculate
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::collectInputs()
{
    auto &t = calc.tpiobj;

    // Personal
    QByteArray ba = edName->text().toUtf8();
    strncpy(t.TaxPayerName, ba.constData(), 99);
    t.Age           = sbAge->value();
    t.FinancialYear = sbFY->value();

    // Salary
    t.Basic          = sbBasic->value();
    t.HRA            = sbHRA->value();
    t.SpecialAllowance = sbSpecial->value();
    t.Bonus          = sbBonus->value();
    t.OtherSalary    = sbOtherSal->value();

    // HRA
    t.MonthlyRentPaid= sbRent->value();
    t.IsMetroCity    = chkMetro->isChecked();
    t.HRAExemption   = calc.CalcHRAExemption();

    // House property
    t.GrossRental   = sbRental->value();
    t.MunicipalTax  = sbMunTax->value();
    t.HomeLoanInt   = sbHomeLoan->value();

    // Capital gains
    t.STCG = sbSTCG->value();
    t.LTCG = sbLTCG->value();

    // Other income
    t.InterestIncome = sbInterest->value();
    t.BusinessIncome = sbBusiness->value();
    t.OtherIncome    = sbOtherInc->value();

    // Old regime deductions
    t.Sec80C          = sb80C->value();
    t.Sec80D          = sb80D->value();
    t.Sec80E          = sb80E->value();
    t.Sec80G          = sb80G->value();
    t.Sec80TTA        = sb80TTA->value();
    t.Sec80CCD1B      = sb80CCD1B->value();
    t.LTAExemption    = sbLTA->value();
    t.OtherDeductions = sbOtherDed->value();

    // New regime deductions
    t.EmployerNPS = sbEmpNPS->value();
    t.AgniVeer    = sbAgniVeer->value();
}

void MainWindow::onCalculate()
{
    collectInputs();

    TaxResult oldR = calc.Compute(false);
    TaxResult newR = calc.Compute(true);

    showResults(oldR, newR);

    // Switch to results tab (index 7)
    auto *tabs = findChild<QTabWidget*>();
    if (tabs) tabs->setCurrentIndex(7);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Slot: Reset
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onReset()
{
    edName->clear();
    sbAge->setValue(30);  sbFY->setValue(2024);

    for (auto *w : {sbBasic, sbHRA, sbSpecial, sbBonus, sbOtherSal,
                    sbRent, sbRental, sbMunTax, sbHomeLoan,
                    sbSTCG, sbLTCG,
                    sbInterest, sbBusiness, sbOtherInc,
                    sb80C, sb80D, sb80E, sb80G, sb80TTA, sb80CCD1B,
                    sbLTA, sbOtherDed, sbEmpNPS, sbAgniVeer})
        w->setValue(0);

    chkMetro->setChecked(false);
    lblHRAExemption->setText("₹ 0.00");
    lblOldTax->setText("₹ 0.00");
    lblNewTax->setText("₹ 0.00");
    lblRecommend->setText("–");
    txtResults->clear();
}

// ─────────────────────────────────────────────────────────────────────────────
//  showResults
// ─────────────────────────────────────────────────────────────────────────────
QString MainWindow::formatMoney(double v)
{
    return QString("₹ %L1").arg(v, 0, 'f', 2);
}

QString MainWindow::buildBreakdown(const TaxResult &r, const QString &regime)
{
    auto row = [](const QString &lbl, double val, int pad = 42) -> QString {
        return QString("  %-*1$s : ₹ %2\n").arg(pad)
                       .arg(lbl, -pad)
                       .replace("%2", QString("%1").arg(val, 12, 'f', 2));
    };

    // Build a clean table using fixed-width formatting
    auto fmtRow = [](const QString &label, double value) -> QString {
        return QString("  %-42s : ₹ %1\n").arg(label, -42)
                       .replace("%1", QString("%1").arg(value, 12, 'f', 2));
    };

    QString out;
    out += QString("═══════════════════════════════════════════════════════════\n");
    out += QString("  %1 REGIME – TAX COMPUTATION\n").arg(regime.toUpper());
    out += QString("═══════════════════════════════════════════════════════════\n");

    out += "\n  INCOME\n";
    out += QString("  ─────────────────────────────────────────────────────\n");
    out += fmtRow("Gross Salary",            r.grossIncome - r.normalTaxable - calc.tpiobj.STCG - calc.tpiobj.LTCG + r.normalTaxable);
    out += fmtRow("(-) Standard Deduction",  r.standardDeduction);
    out += fmtRow("Short-Term Capital Gains",calc.tpiobj.STCG);
    out += fmtRow("Long-Term Capital Gains", calc.tpiobj.LTCG);
    out += fmtRow("Gross Total Income",      r.grossIncome);

    out += "\n  DEDUCTIONS\n";
    out += QString("  ─────────────────────────────────────────────────────\n");
    out += fmtRow("Total Deductions",        r.totalDeductions);
    out += fmtRow("Normal Taxable Income",   r.normalTaxable);
    out += fmtRow("Total Taxable Income",    r.taxableIncome);

    out += "\n  TAX COMPUTATION\n";
    out += QString("  ─────────────────────────────────────────────────────\n");
    out += fmtRow("Slab Tax",                r.slabTax);
    out += fmtRow("STCG Tax (@15%)",         r.stcgTax);
    out += fmtRow("LTCG Tax (@10% >₹1L)",   r.ltcgTax);
    out += fmtRow("Gross Tax Liability",     r.taxBeforeRebate);
    out += fmtRow("(-) Rebate u/s 87A",      r.rebate87A);
    out += fmtRow("Tax after Rebate",        r.taxAfterRebate);
    out += fmtRow("(+) Surcharge",           r.surcharge);
    out += fmtRow("(+) Health & Edu Cess 4%",r.cess);
    out += QString("  ═════════════════════════════════════════════════════\n");
    out += fmtRow("★  TOTAL TAX LIABILITY",  r.totalTax);
    out += QString("  ═════════════════════════════════════════════════════\n");
    out += QString("  %-42s : %1 %%\n").arg("Effective Tax Rate", -42)
                  .replace("%1", QString("%1").arg(r.effectiveRate, 6, 'f', 2));
    out += fmtRow("Monthly TDS (Approx.)",   r.monthlyTDS);
    out += "\n";

    return out;
}

void MainWindow::showResults(const TaxResult &oldR, const TaxResult &newR)
{
    lblOldTax->setText(formatMoney(oldR.totalTax));
    lblNewTax->setText(formatMoney(newR.totalTax));

    double saving = oldR.totalTax - newR.totalTax;
    if (saving > 0.01) {
        lblRecommend->setText("✅ New Regime\nsaves " + formatMoney(saving));
        lblRecommend->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: bold;")
                                   .arg(kAccentGrn));
    } else if (saving < -0.01) {
        lblRecommend->setText("✅ Old Regime\nsaves " + formatMoney(-saving));
        lblRecommend->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: bold;")
                                   .arg(kAccentAmb));
    } else {
        lblRecommend->setText("Both regimes\nare equal");
        lblRecommend->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: bold;")
                                   .arg(kTextSec));
    }

    QString text;
    text += buildBreakdown(oldR, "Old");
    text += buildBreakdown(newR, "New");

    // Comparison table
    text += "═══════════════════════════════════════════════════════════\n";
    text += "  COMPARISON AT A GLANCE\n";
    text += "═══════════════════════════════════════════════════════════\n";
    auto cmpRow = [](const QString &lbl, double v1, double v2) -> QString {
        return QString("  %-26s  %16s  %16s\n")
               .arg(lbl, -26)
               .arg(QString("₹ %1").arg(v1, 10, 'f', 0), 16)
               .arg(QString("₹ %1").arg(v2, 10, 'f', 0), 16);
    };
    text += QString("  %-26s  %16s  %16s\n").arg("Parameter", -26).arg("Old Regime", 16).arg("New Regime", 16);
    text += "  ──────────────────────────────────────────────────────────\n";
    text += cmpRow("Taxable Income",   oldR.taxableIncome,  newR.taxableIncome);
    text += cmpRow("Total Deductions", oldR.totalDeductions,newR.totalDeductions);
    text += cmpRow("Total Tax",        oldR.totalTax,       newR.totalTax);
    text += QString("  %-26s  %15.2f%%  %15.2f%%\n")
            .arg("Effective Rate", -26)
            .arg(oldR.effectiveRate).arg(newR.effectiveRate);
    text += cmpRow("Monthly TDS",      oldR.monthlyTDS,     newR.monthlyTDS);
    text += "═══════════════════════════════════════════════════════════\n";

    txtResults->setPlainText(text);
}
