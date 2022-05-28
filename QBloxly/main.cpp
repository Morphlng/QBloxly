#include "QBloxly.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QBloxly w;
    w.show();
    return a.exec();
}
