#include "mythread.h"

MyThread::MyThread(QObject *parent)
: QThread(parent)
{


}

void MyThread::run()
{
    client = new QTcpSocket(this);
    connect(client, SIGNAL(connected()), this, SLOT(sendCmd()) );
    connect(client, SIGNAL(readyRead()), this, SLOT(readRply()));
    connect(client, SIGNAL(error(QAbstractSocket::SocketError)),
                 this, SLOT(displayError(QAbstractSocket::SocketError)));


       qDebug() << "Executing in new independant thread, GUI is NOT blocked";

       client->abort();
       client->connectToHost( "192.168.1.102", 2000 );
        //client.write( cmd, cmd.length() );
        //client.read( buffer, client.bytesAvailable() );
        //qDebug() << buffer;
        //printf("%s\n",buffer);
        //printf("Ajay\n");
	for(int i=0;i<10;i++)
	{
		qDebug() << "Time: " << 10-i;	


                int t=1;

		// some OS specific stuff
		// mingw (3.4.2) sleep on windows is called _sleep and uses microseconds
		#ifdef Q_OS_WIN32
		t = t * 1000;
		_sleep(t);
		#else
		sleep(t);
		#endif

	}
	
	qDebug() << "Execution done";
	
	exec();
}

void MyThread::sendCmd(void){
    qDebug() << "Sending command\n";
    QByteArray cmd = "*SRTF\r\n";
    client->write( cmd, cmd.length() );
    //client->write("*SRTF\r\n",7);
}

void MyThread::readRply(void){
    char buffer[ 1024 ] = { 0 };
    client->read( buffer, client->bytesAvailable() );
    qDebug() << buffer;
}


void MyThread::displayError(QAbstractSocket::SocketError socketError)
 {
     switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
         break;
     case QAbstractSocket::HostNotFoundError:
         qDebug()<< "The host was not found. Please check the host name and port settings.";
         break;
     case QAbstractSocket::ConnectionRefusedError:
         qDebug()<< "The connection was refused by the peer. Make sure the fortune server is running, and check that the host name and port settings are correct.";
         break;
     default:
         qDebug()<< "The following error occurred: %1.arg(client->errorString()";
     }
 }




