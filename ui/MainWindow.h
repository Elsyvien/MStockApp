#pragma once
#include <QMainWindow>
#include <QStandardItemModel>
#include "PythonBridge.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onData(const QJsonObject& payload);
    void onFailed(const QString& msg);
    void fetchNow();

private:
    Ui::MainWindow *ui;
    QStandardItemModel* model;
    PythonBridge* bridge;
};