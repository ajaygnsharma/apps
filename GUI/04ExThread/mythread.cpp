#include "mythread.h"

MyThread::MyThread(QObject *parent)
: QThread(parent)
{


}

void MyThread::run()
{
    QTcpSocket client;
       qDebug() << "Executing in new independant thread, GUI is NOT blocked";
       connect( &client, SIGNAL(connected()), this, SLOT(sendCmd()) );
       connect( &client, SIGNAL(readyRead()), this, SLOT(readRply()) );

       client.connectToHost( "192.168.1.102", 2000 );
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
    client.write( cmd, cmd.length() );
}

void MyThread::readRply(void){
    char buffer[ 1024 ] = { 0 };
    client.read( buffer, client.bytesAvailable() );
    qDebug() << buffer;
}




