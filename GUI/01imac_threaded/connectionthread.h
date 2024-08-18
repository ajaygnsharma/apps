#ifndef CONNECTIONTHREAD_H
#define CONNECTIONTHREAD_H

#include <QThread>
#include <QtGui>
#include <QtNetwork>
#include <QTcpSocket>
#include <stdio.h>
#include "client.h"


class ConnectionThread : public QThread, public Client
{
    //Q_OBJECT
public:
    ConnectionThread(QObject *parent);
    void run();

private:
    QTcpSocket *client;
    int cmdState;

private slots:
    void startConn();
    void displayConnected();
    void setMac();
    void startRead();
    void displayError(QAbstractSocket::SocketError socketError);

};

#endif // CONNECTIONTHREAD_H
