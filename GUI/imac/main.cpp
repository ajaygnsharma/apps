#include <QtGui/QApplication>
#include "client.h"
#include <QFile>
#include <QTextStream>
using namespace std;

int main(int argc, char *argv[]){

    QApplication a(argc, argv);

    Client w;
    w.show();
    return a.exec();
}
