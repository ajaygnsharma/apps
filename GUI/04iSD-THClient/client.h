#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QDialog>

class QLineEdit;

#define TEMPERATURE_RNDG 	0
#define HUMIDITY_RDNG 		1
#define DEWPOINT_RDNG		2

//class Client: public QObject
class Client: public QDialog
{
Q_OBJECT
public:
    //Client( int sampleRate, QString command, QObject *parent = 0);
	Client( int sampleRate, QString command, QString address, quint16 port  );    
    ~Client();
    void start( QString address, quint16 port );

public slots:
	void startPolling();    
    void startTransfer();
    void startRead();
    void displayConnected();
    void stopPolling();

private:
    QTcpSocket client;
    QString ipAddr;
    quint16 portNum;
    int sampleRate;
    QString command;
    int cmdState;
    QLineEdit *temperatureEdit;
    QLineEdit *humidityEdit;
	QLineEdit *dewpointEdit;
	QTimer *timer;
};

#endif // CLIENT_H

