#include "client.h"
#include <QtCore/QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QApplication>

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    QString ipAddr;
    quint16 portNum;
    int sampleRate;
    QString command;
	
	QApplication a(argc, argv);

    QWidget window;

	//TemperatureConvertor tc;
    //tc.setAttribute(Qt::WA_QuitOnClose);
    //tc.show();
	
    QFile file("/etc/newport/isd_th_client.conf");
    QString line;
    if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        QTextStream t( &file );
		
		line = t.readLine();
        ipAddr = line;
        line = t.readLine();
        portNum = line.toInt();
        line = t.readLine();
        sampleRate = line.toInt();
        line = t.readLine();
        command = line;//.toStdString();
        
		Client client( sampleRate, command, ipAddr, portNum );
        client.setAttribute(Qt::WA_QuitOnClose);		
		client.show();
		
        file.close();
		return a.exec();
		
        
		
        //return a.exec();
    }
    
    
	
	
    //return 0;
}
