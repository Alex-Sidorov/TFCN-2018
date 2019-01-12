#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QString t = "~~~1";
    QString temp;
    QString flag = "01111110";
    code(temp,t,flag);
    QByteArray b = temp.toLatin1();
    temp.clear();
    decode(temp,b);
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    return a.exec();
}
