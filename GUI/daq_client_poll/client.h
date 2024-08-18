#ifndef CLIENT_H
#define CLIENT_H

#include <QtGui/QDialog>


#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QDialog>

class QLineEdit;
class QLabel;

#define TEMPERATURE_RDNG 	0
#define PRESSURE_RDNG 		1


class Client : public QDialog
{
    Q_OBJECT
public:
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
    int     sampleRate;
    QString command;
    int     cmdState;

    QLineEdit *temperatureEdit;
    QLineEdit *pressureEdit;
    QLineEdit *statusEdit;
    QTimer  *timer;
};

#endif // CLIENT_H

