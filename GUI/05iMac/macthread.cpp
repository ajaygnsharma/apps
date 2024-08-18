#include <QtNetwork>
#include "macthread.h"

MacThread::MacThread(QObject *parent)
    : QThread(parent), quit(false){
}


MacThread::~MacThread(){
     mutex.lock();
     quit = true;
     cond.wakeOne();
     mutex.unlock();
     wait();
}

void MacThread::connectSetMac(const QString &hostName, const QString &macAddrStr){
     QMutexLocker locker(&mutex);
     this->hostName = hostName;
     this->port = 2002;
     this->macAddrStr = macAddrStr;
     if (!isRunning())
         start();
     else
         cond.wakeOne();
}

 void MacThread::run()
 {
     mutex.lock();
     QString serverName = hostName;
     quint16 serverPort = port;
     QString serverMacAddrStr = macAddrStr;
     mutex.unlock();

     while (!quit) {
         const int Timeout = 5 * 1000;

         QTcpSocket socket;
         socket.connectToHost(serverName, serverPort);

         if (!socket.waitForConnected(Timeout)) {
             emit error(socket.error(), socket.errorString());
             return;
         }

         if (!socket.waitForReadyRead(Timeout)) {
            emit error(socket.error(), socket.errorString());
            return;
         }

         mutex.lock();
         QString macStr;
         macStr = socket.read(40);

         QByteArray cmd = "00000000\r\n";
         if(socket.write(cmd,cmd.length()) == -1){
            emit newStatus("cannot write to socket");
         }
         if(!socket.waitForBytesWritten((Timeout))){
             emit error(socket.error(), socket.errorString());
             return;
         }

         if (!socket.waitForReadyRead(Timeout)) {
             emit error(socket.error(), socket.errorString());
             return;
         }

         cmd = "s -MC";
         cmd.append(serverMacAddrStr);
         cmd.append("\n");
         if(socket.write(cmd,cmd.length()) == -1){
            emit newStatus("cannot write to socket");
         }
         if(!socket.waitForBytesWritten((Timeout))){
             emit error(socket.error(), socket.errorString());
             return;
         }
         if (!socket.waitForReadyRead(Timeout)) {
             emit error(socket.error(), socket.errorString());
             return;
         }
         QString replyStr;
         replyStr = socket.read(40);
         emit newStatus(replyStr);


         cmd = "c\n";
         if(socket.write(cmd,cmd.length()) == -1){
            emit newStatus("cannot write to socket");
         }
         if(!socket.waitForBytesWritten((Timeout))){
             emit error(socket.error(), socket.errorString());
             return;
         }
         if (!socket.waitForReadyRead(Timeout)) {
             emit error(socket.error(), socket.errorString());
             return;
         }
         replyStr = socket.read(40);
         emit newStatus(replyStr);

         cmd = "RESET\n";
         if(socket.write(cmd,cmd.length()) == -1){
            emit newStatus("cannot write to socket");
         }
         if(!socket.waitForBytesWritten((Timeout))){
             emit error(socket.error(), socket.errorString());
             return;
         }
         if (!socket.waitForReadyRead(Timeout)) {
             emit error(socket.error(), socket.errorString());
             return;
         }
         replyStr = socket.read(40);
         emit newStatus(replyStr);

         cond.wait(&mutex);
         serverName = hostName;
         serverPort = port;
         mutex.unlock();
     }
 }






/*void ConnectionThread::startRead(){
    char buffer[ 1024 ] = { 0 };
    QString qRespBuff;
    int retVal = 0;
    string respStr;
    //QByteArray cmd;
    QPalette pal;
    pal.setColor( QPalette::Text, Qt::darkGreen);

    switch( cmdState ){
    case INITIAL:
        client.read( buffer, client.bytesAvailable() );
        qRespBuff.insert( 0, buffer );
        retVal = qRespBuff.compare( qRespBuff, "Administrator Password?" );

        if( retVal == 0 ){
            cmdState = PASSWORD_OK;
            cmd = "00000000\n";
            client.write( cmd, cmd.length() );
        }else{
            cmdState = INITIAL;
        }
        break;

    case ENTER_PASSWORD:
        break;

    case PASSWORD_OK:
        client.read( buffer, client.bytesAvailable() );
        qRespBuff.insert( 0, buffer );
            retVal = qRespBuff.endsWith( "SD>" );
        if( retVal != string::npos ){
            cmdState = SET_MAC;
        }else{
            cmdState = ENTER_PASSWORD;
        }
        break;

    case SET_MAC:
        break;

    case SET_MAC_CMD_OK:
        client.read( buffer, client.bytesAvailable() );
        qRespBuff.insert( 0, buffer );
            retVal = qRespBuff.startsWith("CMD_OK");
        if( retVal == TRUE ){
            cmd = "c\n";
            client.write( cmd, cmd.length() );
            cmdState = SET_MAC_PERM_OK;
        }else{
            retVal = qRespBuff.startsWith("WRONG_ARG_OR_VALUE" );
            if( retVal == TRUE ){
                cmdState = SET_MAC;
                statusEdit->setPalette( pal );
                statusEdit->setText( "Error: Chk Mac Addr" );
            }
        }
        break;

     case SET_MAC_PERM:
        break;

     case SET_MAC_PERM_OK:
        client.read( buffer, client.bytesAvailable() );
        qRespBuff.insert( 0, buffer );
            retVal = qRespBuff.startsWith("SAVE_OK");
        if( retVal == TRUE ){
            cmdState = RESET_ST;
            cmd = "RESET\n";
            client.write( cmd, cmd.length() );
        }else{
            cmdState = SET_MAC_PERM_OK;
        }
        break;

     case CLOSE_SOCK:
        break;

     case RESET_ST:
        client.read( buffer, client.bytesAvailable() );
        qRespBuff.insert( 0, buffer );
        retVal = qRespBuff.startsWith("PLEASE_WAIT_15_SECONDS_FOR_DEVICE_TO_REBOOT");
        if( retVal == TRUE ){
            statusEdit->setPalette( pal );
            statusEdit->setText( "Done,Rebooting.." );
            client.close();
            cmdState = INITIAL;
        }else{
            cmdState = SET_MAC_PERM_OK;
        }
        break;
    }
    respStr = qRespBuff.toStdString();

    cout<<respStr<<endl;
}


void ConnectionThread::setMac( void ){
    QByteArray macAddr, cmd;

    switch( cmdState ){
    case SET_MAC:
        cmd = "s -MC";
        macAddr.insert( 0, macEdit->text() );
        client.write( cmd, cmd.length() );
        client.write( macAddr, macAddr.length() );
        client.write( "\n", 1 );
        cmdState = SET_MAC_CMD_OK;
        break;

    default:
        break;
    }
    return;
}

void ConnectionThread::startConn( void ){
    QHostAddress addr( ipEdit->text() );
    client.connectToHost( addr, 2002 );
}


void ConnectionThread::displayConnected( void ){
    statusEdit->setDisabled(TRUE);
    macEdit->setEnabled(TRUE);
    statusEdit->setText( "Connected" );
    cmdState = INITIAL;
}

void ConnectionThread::displayError( void ){
    statusEdit->setDisabled(TRUE);
    macEdit->setDisabled(TRUE);
    statusEdit->setText( "Cannot connect" );
    cmdState = INITIAL;
    client.close();
}
*/
