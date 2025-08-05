//
// Created by max on 05.08.25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MStock.h" resolved

#include "mstock.h"
#include "ui_MStock.h"


MStock::MStock(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MStock) {
    ui->setupUi(this);
}

MStock::~MStock() {
    delete ui;
}
