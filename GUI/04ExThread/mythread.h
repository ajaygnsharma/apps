#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QtGui>
#include <QtNetwork>
#include <QTcpSocket>

#include <stdio.h>

class MyThread : public QThread
{
	Q_OBJECT

public:
	MyThread(QObject *parent);
	void run(); // this is virtual method, we must implement it in our subclass of QThread

public slots:
        void sendCmd(void);
        void readRply(void);


private:
    QTcpSocket client;
};


#endif
