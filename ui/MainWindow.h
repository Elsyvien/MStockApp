#pragma once
#include <QMainWindow>
#include <QJsonObject>

class QLineEdit;
class QPushButton;
class QTableView;
class QStandardItemModel;
class PythonBridge;
class GraphWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void fetchNow();
    void onData(const QJsonObject& payload);
    void onFailed(const QString& msg);

private:
    QLineEdit*           searchBar   = nullptr;
    QPushButton*         fetchButton = nullptr;
    QTableView*          table       = nullptr;
    QStandardItemModel*  model       = nullptr;
    PythonBridge*        bridge      = nullptr;
    GraphWidget*         graph       = nullptr;  // <— wichtig
};
