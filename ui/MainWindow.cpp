#include "MainWindow.h"
#include "PythonBridge.h"
#include "GraphWidget.h"

#include <QApplication>
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
#include <QItemSelectionModel>
#include <QJsonObject>
#include <QJsonArray>

#include <limits>

#ifndef PYTHON_EXE
#define PYTHON_EXE "/usr/bin/python3"
#endif
#ifndef SCRIPT_NAME
#define SCRIPT_NAME "fetch_stock.py"
#endif

// ----- Helpers -----
static QStringList parseTickers(const QString& text) {
    QString t = text;
    t.replace(';', ',');
    QStringList parts = t.split(QRegularExpression("[,\\s]+"), Qt::SkipEmptyParts);
    for (QString& s : parts) s = s.trimmed().toUpper();
    parts.removeAll(QString());
    return parts;
}

// ----- MainWindow -----
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto *central    = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    // Topbar: User Menu
    auto *userMenu   = new QMenu("Benutzer", this);
    userMenu->addAction("Einstellungen");
    userMenu->addAction("Über");
    QAction* quitAction = userMenu->addAction("Beenden");
    auto *userButton = new QPushButton("Nutzer");
    userButton->setMenu(userMenu);
    userButton->setStyleSheet(
        "font-size: 16px; background-color: #E0E0E0; color: black; "
        "border-radius: 12px; QPushButton::menu-indicator { image: none; }");
    userButton->setFixedSize(120, 40);
    auto *userLayout = new QHBoxLayout();
    userLayout->addStretch();
    userLayout->addWidget(userButton);
    mainLayout->addLayout(userLayout);

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
    searchBar->setStyleSheet(
        "font-size: 15px; padding: 4px 8px; border-radius: 6px; border: 1px solid #bbb;");
    searchLayout->addWidget(searchBar);
    mainLayout->addLayout(searchLayout);

    // Fetch-Button
    fetchButton = new QPushButton("Mein Portfolio aktualisieren", central);
    fetchButton->setFixedSize(260, 40);
    fetchButton->setStyleSheet(
        "font-size: 16px; background-color: #1976D2; color: white; border-radius: 12px;");
    mainLayout->addWidget(fetchButton, 0, Qt::AlignCenter);

    // Table
    table = new QTableView(central);
    table->setMinimumHeight(220);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mainLayout->addWidget(table);

    // Graph
    graph = new GraphWidget(central);
    mainLayout->addWidget(graph);

    // Window setup
    mainLayout->setAlignment(Qt::AlignTop);
    central->setLayout(mainLayout);
    setCentralWidget(central);
    setWindowTitle("MStockApp");
    resize(900, 700);

    // Model + Bridge
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Ticker","Last","Currency","Status"});
    table->setModel(model);

    bridge = new PythonBridge(this);
    connect(bridge, &PythonBridge::received, this, &MainWindow::onData);
    connect(bridge, &PythonBridge::failed,   this, &MainWindow::onFailed);

    // Interactions
    connect(fetchButton, &QPushButton::clicked,    this, &MainWindow::fetchNow);
    connect(searchBar,   &QLineEdit::returnPressed,this, &MainWindow::fetchNow);
    connect(quitAction,  &QAction::triggered,      qApp, &QCoreApplication::quit);

    // Table Selection
    connect(table->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, [this](const QModelIndex& current, const QModelIndex&) {
                if (!current.isValid()) return;
                const QString ticker = model->item(current.row(), 0)->text();
                const QString scriptPath = QCoreApplication::applicationDirPath() + "/" + SCRIPT_NAME;
                bridge->requestHistory(ticker, QStringLiteral(PYTHON_EXE), scriptPath, "1mo", "1d");
            });

    // Defaults
    searchBar->setText("AAPL, MSFT, NVDA, GOOGL");

    // First Call + Periodic Updates
    fetchNow();
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::fetchNow);
    timer->start(60'000);
}

MainWindow::~MainWindow() = default;

// ----- Actions -----
void MainWindow::fetchNow() {
    const QStringList tickers = parseTickers(searchBar->text());
    if (tickers.isEmpty()) {
        statusBar()->showMessage("Kein Ticker angegeben.", 2500);
        return;
    }
    const QString scriptPath = QCoreApplication::applicationDirPath() + "/" + SCRIPT_NAME;
    bridge->requestPrices(tickers, QStringLiteral(PYTHON_EXE), scriptPath);

    // Optional: Load history for the first ticker directly (faster user experience)
    // bridge->requestHistory(tickers.first(), QStringLiteral(PYTHON_EXE), scriptPath, "1mo", "1d");

    statusBar()->showMessage("Fetching…", 1200);
}

// ----- Data handling -----
void MainWindow::onData(const QJsonObject& payload) {
    // Support "type": "prices" | "history"; Default = prices (for older scripts)
    const QString type = payload.value("type").toString("prices");

    if (type == "history") {
        const QString ticker = payload.value("ticker").toString();
        const QJsonObject seriesObj = payload.value("series").toObject();
        if (seriesObj.contains("error")) {
            statusBar()->showMessage(
                QString("History-Fehler %1: %2")
                    .arg(ticker, seriesObj.value("error").toString()),
                4000);
            return;
        }
        const QJsonArray pts = seriesObj.value("points").toArray();
        QVector<QPointF> data; data.reserve(pts.size());
        for (const QJsonValue& v : pts) {
            const QJsonArray p = v.toArray();
            if (p.size() != 2) continue;
            data.append(QPointF(p[0].toDouble(), p[1].toDouble())); // x=ms epoch, y=close
        }
        graph->setSeries(data, ticker);
        statusBar()->showMessage(QString("Chart %1: %2 Punkte").arg(ticker).arg(data.size()), 2000);
        return;
    }

    // --- Preise-Tabelle aktualisieren ---
    const QJsonObject dataObj = payload.value("data").toObject();
    model->removeRows(0, model->rowCount());

    for (auto it = dataObj.begin(); it != dataObj.end(); ++it) {
        const QString t = it.key();
        const QJsonObject v = it.value().toObject();

        QList<QStandardItem*> row;
        if (v.contains("error")) {
            row << new QStandardItem(t)
                << new QStandardItem("-")
                << new QStandardItem("-")
                << new QStandardItem(v.value("error").toString());
        } else {
            double last = std::numeric_limits<double>::quiet_NaN();
            const QJsonValue lastVal = v.value("last");
            if (lastVal.isDouble()) {
                last = lastVal.toDouble();
            } else if (lastVal.isString()) {
                bool ok=false;
                const double tmp = lastVal.toString().toDouble(&ok);
                if (ok) last = tmp;
            }
            const QString lastText = std::isnan(last) ? QStringLiteral("-")
                                                      : QString::number(last, 'f', 2);
            const QString currency = v.value("currency").toString();

            row << new QStandardItem(t)
                << new QStandardItem(lastText)
                << new QStandardItem(currency.isEmpty() ? "-" : currency)
                << new QStandardItem("OK");
        }
        model->appendRow(row);
    }

    // Automatically select the first row on the first update → loads history via selection handler
    if (model->rowCount() > 0 && !table->selectionModel()->hasSelection()) {
        table->selectRow(0);
    }

    const int ts = payload.value("ts").toInt();
    statusBar()->showMessage(
        QStringLiteral("Updated %1")
            .arg(QDateTime::fromSecsSinceEpoch(ts).toString()),
        3000);
}

void MainWindow::onFailed(const QString& msg) {
    statusBar()->showMessage(msg, 5000);
    qWarning().noquote() << "[bridge failed]" << msg;
}
