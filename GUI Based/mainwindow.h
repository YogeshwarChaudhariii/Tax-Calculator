#pragma once

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QTabWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include "TaxCalculator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCalculate();
    void onReset();
    void onRentChanged(double rent);   // auto-recalc HRA exemption live

private:
    // ── UI helpers ────────────────────────────────────────────────────────────
    void buildUI();
    void buildPersonalTab(QTabWidget *tabs);
    void buildSalaryTab(QTabWidget *tabs);
    void buildPropertyTab(QTabWidget *tabs);
    void buildCapitalTab(QTabWidget *tabs);
    void buildOtherIncomeTab(QTabWidget *tabs);
    void buildDeductionsOldTab(QTabWidget *tabs);
    void buildDeductionsNewTab(QTabWidget *tabs);
    void buildResultsTab(QTabWidget *tabs);

    void collectInputs();
    void showResults(const TaxResult &oldR, const TaxResult &newR);
    QString formatMoney(double v);
    QString buildBreakdown(const TaxResult &r, const QString &regime);

    // ── Business logic ────────────────────────────────────────────────────────
    TaxCalculator calc;

    // ── Input widgets (Personal) ──────────────────────────────────────────────
    QLineEdit     *edName;
    QSpinBox      *sbAge;
    QSpinBox      *sbFY;

    // ── Salary ────────────────────────────────────────────────────────────────
    QDoubleSpinBox *sbBasic, *sbHRA, *sbSpecial, *sbBonus, *sbOtherSal;

    // ── HRA ──────────────────────────────────────────────────────────────────
    QDoubleSpinBox *sbRent;
    QCheckBox      *chkMetro;
    QLabel         *lblHRAExemption;

    // ── House Property ────────────────────────────────────────────────────────
    QDoubleSpinBox *sbRental, *sbMunTax, *sbHomeLoan;

    // ── Capital Gains ─────────────────────────────────────────────────────────
    QDoubleSpinBox *sbSTCG, *sbLTCG;

    // ── Other Income ─────────────────────────────────────────────────────────
    QDoubleSpinBox *sbInterest, *sbBusiness, *sbOtherInc;

    // ── Deductions Old ────────────────────────────────────────────────────────
    QDoubleSpinBox *sb80C, *sb80D, *sb80E, *sb80G, *sb80TTA, *sb80CCD1B;
    QDoubleSpinBox *sbLTA, *sbOtherDed;

    // ── Deductions New ────────────────────────────────────────────────────────
    QDoubleSpinBox *sbEmpNPS, *sbAgniVeer;

    // ── Results ───────────────────────────────────────────────────────────────
    QTextEdit      *txtResults;
    QLabel         *lblOldTax, *lblNewTax, *lblRecommend;
    QPushButton    *btnCalc, *btnReset;
};
