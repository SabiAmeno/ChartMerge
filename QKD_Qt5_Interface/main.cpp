#include "QKD_Qt5_Interface.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QKD_Qt5_Interface w;
    w.show();
    return a.exec();
}
