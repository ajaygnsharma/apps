#include "client.h"
#include <QtCore/QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Client client;
    client.start( "192.168.1.2", 2000 );

    return a.exec();
}
