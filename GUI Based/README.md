# Income Tax Calculator – Qt GUI (FY 2024-25)

A full-featured Qt Widgets GUI for the Income Tax Calculator.
Converted from `p1.cpp` (CLI) to a tabbed Qt desktop application.

---

## Project Structure

```
TaxCalculatorQt/
├── TaxCalculatorQt.pro   Qt project file
├── TaxCalculator.h       Pure C++ tax logic (no Qt dependency)
├── mainwindow.h          MainWindow class declaration
├── mainwindow.cpp        Full UI + event handling
├── mainwindow.ui         Placeholder .ui file
├── main.cpp              Qt entry point
└── README.md             This file
```

---

## Features

| Tab | Contents |
|-----|----------|
| Personal | Name, Age, Financial Year |
| Salary | Basic, HRA, Special Allowance, Bonus – with live HRA exemption |
| Property | Rental income, Municipal tax, Home Loan Interest (Sec 24b) |
| Cap. Gains | STCG @15%, LTCG @10% above ₹1L |
| Other Income | FD interest, Business income, Others |
| Ded. (Old) | 80C, 80D, 80E, 80G, 80TTA, 80CCD(1B), LTA, Others |
| Ded. (New) | Employer NPS (80CCD-2), Agniveer Corpus |
| Results | Side-by-side comparison + detailed breakdown |

### Tax Logic (FY 2024-25)
- **Old Regime** – Standard deduction ₹50,000; exemptions at ₹2.5L / ₹3L (60+) / ₹5L (80+); Rebate u/s 87A up to ₹12,500 if income ≤ ₹5L
- **New Regime** – Standard deduction ₹75,000; uniform slabs 0→3L→7L→10L→12L→15L→30%; Rebate u/s 87A up to ₹25,000 if income ≤ ₹7L
- **Capital Gains** taxed separately (not mixed with slab)
- Surcharge (10% / 15% / 25% / 37% old, 25% new above ₹5Cr)
- Health & Education Cess: 4%

---

## Notes
- `TaxCalculator.h` has **zero Qt dependency** — the same header can be reused in the original CLI version.
- The UI is built entirely in C++ code (no `.ui` drag-and-drop widgets) for easy reading and modification.
