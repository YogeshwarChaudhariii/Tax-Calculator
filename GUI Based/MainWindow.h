#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QTextEdit>
#include <QButtonGroup>

#include "taxpayerinfo.h"
#include "taxcalculator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onCalculate();
    void onReset();
    void onRentToggled(double rent);

private:
    // ── Collect form data ──
    TaxPayerInfo collectData() const;

    // ── Build UI sections ──
    QWidget* buildInputPage();
    QWidget* buildResultPage();
    QWidget* AboutUs();
    QWidget* HelpPage();

    QGroupBox* buildPersonalGroup();
    QGroupBox* buildSalaryGroup();
    QGroupBox* buildHRAGroup();
    QGroupBox* buildOtherIncomeGroup();
    QGroupBox* buildOldDeductionsGroup();
    QGroupBox* buildNewDeductionsGroup();

    QString formatResult(const QString& name, int age,
        const RegimeResult& oldR,
        const RegimeResult& newR,
        int choice) const;

    // ── Widgets ──
    QTabWidget* m_tabs;

    // Personal
    QLineEdit* m_name;
    QSpinBox* m_age;

    // Salary
    QDoubleSpinBox* m_basic, * m_hra, * m_special, * m_bonus;

    // HRA
    QDoubleSpinBox* m_rent;
    QCheckBox* m_metro;

    // Other income
    QDoubleSpinBox* m_rentalIncome, * m_stcg, * m_ltcg, * m_otherIncome;

    // Old regime deductions
    QDoubleSpinBox* m_sec80C, * m_sec80D, * m_sec80E, * m_homeLoan, * m_otherDed;

    // New regime deductions
    QDoubleSpinBox* m_nps;

    // Regime choice
    QRadioButton* m_rbOld, * m_rbNew, * m_rbBoth;

    // Result
    QTextEdit* m_result;
    QPushButton* m_calcBtn;

    // Help Page
    QTextEdit* m_querys;

    // About us
    // Help Page
    QTextEdit* m_about;
};

#endif // MAINWINDOW_H