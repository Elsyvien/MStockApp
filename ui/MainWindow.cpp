#include "MainWindow.h"
#include "PythonBridge.h"

#include <QMenu>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QTimer>
#include <QDateTime>
#include <QStringList>
#include <QDebug>
#include <QStatusBar>
#include <qjsonvalue.h>
#include <QJsonObject>

#include <limits>

#ifndef PYTHON_EXE
#define PYTHON_EXE "/usr/bin/python"
#endif
#ifndef SCRIPT_NAME
#define SCRIPT_NAME "fetch_stock.py"
#endif

static QStringList parseTickers(const QString& text) {
    QString t = text;
    t.replace(';', ',');
    QStringList parts = t.split(QRegularExpression("[, \\s]+"), Qt::SkipEmptyParts);
    for (QString& s : parts) s = s.trimmed().toUpper();
    parts.removeAll(QString());
    return parts;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    // Title
    auto *label = new QLabel("Willkommen zur MStockApp!", central);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 20px; font-weight: bold; margin: 10px;");
    mainLayout->addWidget(label);

    // Search Bar
    auto *searchLayout = new QHBoxLayout();
    searchBar = new QLineEdit(central);
    searchBar->setPlaceholderText("Aktien als Liste: z. B. AAPL, MSFT, NVDA");
    searchBar->setFixedHeight(32);
    searchBar->setStyleSheet("font-size: 15px; padding: 4px 8px; border-radius: 6px; border: 1px solid #bbb;");
    searchLayout->addWidget(searchBar);
    mainLayout->addLayout(searchLayout);

    // Fetch-Button
    fetchButton = new QPushButton("Mein Portfolio aktualisieren", central);
    fetchButton->setFixedSize(260, 40);
    fetchButton->setStyleSheet("font-size: 16px; background-color: #1976D2; color: white; border-radius: 12px;");
    mainLayout->addWidget(fetchButton, 0, Qt::AlignCenter);

    QAction* quitAction = nullptr;
    // User Menu
    auto *userMenu = new QMenu("Benutzer", this);
    userMenu->addAction("Einstellungen");
    userMenu->addAction("Über");
    quitAction = userMenu->addAction("Beenden");
    auto *userButton = new QPushButton("Nutzer");
    userButton->setMenu(userMenu);
    userButton->setStyleSheet("font-size: 16px; background-color: #E0E0E0; color: black; border-radius: 12px; QPushButton::menu-indicator { image: none; }");
    userButton->setFixedSize(120, 40);
    auto *userLayout = new QHBoxLayout();
    userLayout->addStretch();
    userLayout->addWidget(userButton);
    mainLayout->insertLayout(0, userLayout);

    // Tabel
    table = new QTableView(central);
    table->setMinimumHeight(200);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(table);

    mainLayout->setAlignment(Qt::AlignTop);
    central->setLayout(mainLayout);
    setCentralWidget(central);
    setWindowTitle("MStockApp");
    resize(640, 480);

    // Model + Bridge
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Ticker","Last","Currency","Status"});
    table->setModel(model);

    bridge = new PythonBridge(this);
    connect(bridge, &PythonBridge::received, this, &MainWindow::onData);
    connect(bridge, &PythonBridge::failed, this, &MainWindow::onFailed);

    // Interaction
    connect(fetchButton, &QPushButton::clicked, this, &MainWindow::fetchNow);
    connect(searchBar, &QLineEdit::returnPressed, this, &MainWindow::fetchNow);

    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

    // Default
    searchBar->setText("AAPL, MSFT, NVDA, GOOGL");

    // First call + Periodic call
    fetchNow();
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::fetchNow);
    timer->start(60'000); // every 60s
}

MainWindow::~MainWindow() = default;

void MainWindow::fetchNow() {
    const QStringList tickers = parseTickers(searchBar->text());
    if (tickers.isEmpty()) {
        statusBar()->showMessage("Kein Ticker angegeben.", 2500);
        return;
    }
    const QString scriptPath = QCoreApplication::applicationDirPath() + "/" + SCRIPT_NAME;
    bridge->requestPrices(tickers, QStringLiteral(PYTHON_EXE), scriptPath);
    statusBar()->showMessage("Fetching…", 1200);
}


void MainWindow::onData(const QJsonObject& payload) {
    const auto dataObj = payload.value("data").toObject();

    model->removeRows(0, model->rowCount());

    for (auto it = dataObj.begin(); it != dataObj.end(); ++it) {
        const QString t = it.key();
        const auto v = it.value().toObject();

        QList<QStandardItem*> row;

        if (v.contains("error")) {
            row << new QStandardItem(t)
                << new QStandardItem("-")
                << new QStandardItem("-")
                << new QStandardItem(v.value("error").toString());
        } else {
            // Convert Last to double
            double last = std::numeric_limits<double>::quiet_NaN();
            const QJsonValue lastVal = v.value("last");

            if (lastVal.isDouble()) {
                last = lastVal.toDouble();
            } else if (lastVal.isString()) {
                bool ok = false;
                const double tmp = lastVal.toString().toDouble(&ok);
                if (ok) last = tmp;
            }
            const QString lastText = std::isnan(last) ? QStringLiteral("-") : QString::number(last, 'f', 2);
            const QString currency = v.value("currency").toString();

            row << new QStandardItem(t)
                << new QStandardItem(lastText)
                << new QStandardItem(currency.isEmpty() ? "-" : currency)
                << new QStandardItem("OK");
        }
        model->appendRow(row);
    }
}
void MainWindow::onFailed(const QString& msg) {
    statusBar()->showMessage(msg, 5000);
    qWarning().noquote() << "[bridge failed]" << msg;
}
