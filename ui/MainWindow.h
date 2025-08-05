#pragma once
#include <QMainWindow>
#include <QStandardItemModel>

class QStandardItemModel;
class QTableView;
class QLineEdit;
class QPushButton;
class PythonBridge;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onData(const QJsonObject& payload);
    void onFailed(const QString& msg);
    void fetchNow();

private:
    QLineEdit* searchBar = nullptr;
    QPushButton* fetchButton = nullptr;
    QTableView* table = nullptr;
    QStandardItemModel* model = nullptr;
    PythonBridge* bridge = nullptr;
};