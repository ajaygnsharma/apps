#include <QtGui/QApplication>
#include "client.h"
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QString ipAddr;
    quint16 portNum;
    int sampleRate;
    QString command;

    QFile file("ibtxcl.conf");
    QString line;
    if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ){
        QApplication a(argc, argv);

        QTextStream t( &file );

        line = t.readLine();
        ipAddr = line;
        line = t.readLine();
        portNum = line.toInt();
        line = t.readLine();
        sampleRate = line.toInt();
        line = t.readLine();
        command = line;

        Client client( sampleRate, command, ipAddr, portNum );
        client.start( ipAddr, portNum );
        client.show();
        return a.exec();
    }

    return 0;
}
