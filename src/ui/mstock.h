//
// Created by max on 05.08.25.
//

#ifndef MSTOCK_H
#define MSTOCK_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MStock; }
QT_END_NAMESPACE

class MStock : public QMainWindow {
Q_OBJECT

public:
    explicit MStock(QWidget *parent = nullptr);
    ~MStock() override;

private:
    Ui::MStock *ui;
};


#endif //MSTOCK_H
