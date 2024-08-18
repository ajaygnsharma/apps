//Ajay Sharma
//2010/03/12: Can send command to a designated IP address and get a reply
// Also able to do this on and on every 2 seconds
#include "client.h"
#include "QHostAddress"
#include <iostream>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>


using namespace std;

Client::Client( int sampleRate, QString command, QString ipAddr, quint16 portNum )
{
    this->sampleRate = 1000 * sampleRate;
    this->command = command;
    this->cmdState = TEMPERATURE_RDNG;
    this->start( ipAddr, portNum );
	
    connect( &client, SIGNAL( connected() ), this, SLOT( displayConnected() ) );
    connect( &client, SIGNAL( readyRead() ), this, SLOT( startRead() ) );
    
    QVBoxLayout *mainLayout = new QVBoxLayout( this );
    QGridLayout *editLayout = new QGridLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    mainLayout->addLayout(editLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    QLabel *temperatureLabel = new QLabel(tr("Temperature"));
    QLabel *pressureLabel = new QLabel(tr("Pressure"));
    QLabel *statusLabel = new QLabel(tr("Status"));
    QLabel *statusLabelVal = new QLabel(tr(""));

    temperatureEdit = new QLineEdit;
    pressureEdit = new QLineEdit;

    editLayout->addWidget(temperatureLabel, 0, 0);
    editLayout->addWidget(temperatureEdit, 0, 1);
    editLayout->addWidget(pressureLabel, 1, 0);
    editLayout->addWidget(pressureEdit, 1, 1);
    editLayout->addWidget(statusLabel, 2, 0);
    editLayout->addWidget(statusLabelVal, 2, 1);

    QPushButton *startPolling = new QPushButton(tr("Start Polling"));
    QPushButton *stopPolling = new QPushButton(tr("Stop Polling"));
	
    buttonLayout->addStretch();
    buttonLayout->addWidget( startPolling );
    buttonLayout->addWidget( stopPolling );

    startPolling->setDefault(true);
    setWindowTitle(tr("Weather station"));
    connect( startPolling, SIGNAL(clicked()), this, SLOT( startPolling()));
    connect( stopPolling, SIGNAL(clicked()), this, SLOT( stopPolling() ) );
	            
}

Client::~Client()
{
    client.close();
}

void Client::start( QString address, quint16 port )
{
    QHostAddress addr( address );
    client.connectToHost( addr, port );    
}

void Client::displayConnected( void )
{
    statusLabelVal->setText( "Connected" );
}

void Client::startPolling( void )
{
    cout << "start transfer clicked " << endl;

    QTimer *timer = new QTimer(this);
    connect( timer, SIGNAL( timeout() ), this, SLOT( startTransfer() ) );
    timer->start( Client::sampleRate );
}

void Client::stopPolling( void )
{
    cout << "stop transfer clicked " << endl;
    this->timer->stop();
}

void Client::startTransfer()
{
    QByteArray cmd;
	
    switch( this->cmdState )
    {
        case TEMPERATURE_RDNG:
            cmd = "*SRTF\n";
            break;
    	
        case PRESSURE_RDNG:
            cmd = "*SRHb\n";
            break;
    		
    }
    client.write( cmd, cmd.length() );
    return;
}

void Client::startRead()
{
    char buffer[ 1024 ] = { 0 };

    client.read( buffer, client.bytesAvailable() );
    cout<<buffer<<endl;
    
    switch( this->cmdState )
    {
        case TEMPERATURE_RDNG:
                temperatureEdit->setText( buffer );
                this->cmdState = PRESSURE_RDNG;
                break;
    	
        case PRESSURE_RDNG:
                pressureEdit->setText( buffer );
                this->cmdState = TEMPERATURE_RDNG;
                break;
    		
    }
}

