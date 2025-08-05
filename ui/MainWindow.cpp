#include "MainWindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    // Titel
    auto *label = new QLabel("Willkommen zur MStockApp!", central);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 20px; font-weight: bold; margin: 10px;");
    mainLayout->addWidget(label);

    // Suchleiste für Aktien
    auto *searchLayout = new QHBoxLayout();
    auto *searchBar = new QLineEdit(central);
    searchBar->setPlaceholderText("Aktie suchen...");
    searchBar->setFixedHeight(32);
    searchBar->setStyleSheet("font-size: 15px; padding: 4px 8px; border-radius: 6px; border: 1px solid #bbb;");
    searchLayout->addWidget(searchBar);
    mainLayout->addLayout(searchLayout);

    // Portfolio-Button
    auto *portfolioButton = new QPushButton("Mein Portfolio", central);
    portfolioButton->setFixedSize(160, 40);
    portfolioButton->setStyleSheet("font-size: 16px; background-color: #1976D2; color: white; border-radius: 8px;");
    mainLayout->addWidget(portfolioButton, 0, Qt::AlignCenter);

    mainLayout->setAlignment(Qt::AlignTop);
    central->setLayout(mainLayout);
    setCentralWidget(central);
    setWindowTitle("MStockApp");
    resize(420, 260);
}

MainWindow::~MainWindow() = default;
