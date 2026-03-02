#include "pch.h"
#include <QtWidgets>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Indian Tax Calculator");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("TaxCalc");

    // Modern fusion style
    app.setStyle("Fusion");

    QPalette dark;
    dark.setColor(QPalette::Window, QColor(30, 30, 46));
    dark.setColor(QPalette::WindowText, QColor(205, 214, 244));
    dark.setColor(QPalette::Base, QColor(24, 24, 37));
    dark.setColor(QPalette::AlternateBase, QColor(30, 30, 46));
    dark.setColor(QPalette::ToolTipBase, Qt::white);
    dark.setColor(QPalette::ToolTipText, Qt::white);
    dark.setColor(QPalette::Text, QColor(205, 214, 244));
    dark.setColor(QPalette::Button, QColor(49, 50, 68));
    dark.setColor(QPalette::ButtonText, QColor(205, 214, 244));
    dark.setColor(QPalette::BrightText, Qt::red);
    dark.setColor(QPalette::Link, QColor(137, 180, 250));
    dark.setColor(QPalette::Highlight, QColor(25, 118, 210));
    dark.setColor(QPalette::HighlightedText, Qt::white);
    app.setPalette(dark);

    MainWindow w;
    w.show();
    return app.exec();
}