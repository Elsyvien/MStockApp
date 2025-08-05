#include <QApplication>
#include "ui/mstock.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MStock window;
    window.show();

    return app.exec();
}
