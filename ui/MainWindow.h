#pragma once
#include <QMainWindow>
#include <QStandardItemModel>
#include "PythonBridge.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
};
