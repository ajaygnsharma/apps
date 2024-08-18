#include "client.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPalette>
#include <QtGui>

//iTHX-SD conversation
//1.03a3
//
//Administrator Password?00000000
//
//iTHX-SD>s -MC00:03:34:00:01:02
//CMD_OK
//iTHX-SD>c
//SAVE_OK
//iTHX-SD>q
//QUIT

using namespace std;

Client::Client(QWidget *parent){
    //this->cmdState = INITIAL;
    this->modelType = ITHX_SD;

    QWidget *widget = new QWidget;
    setCentralWidget(widget);
    QWidget *topFiller = new QWidget;
    topFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);    
    QWidget *bottomFiller = new QWidget;
    bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QGridLayout *editLayout = new QGridLayout;
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    mainLayout->setMargin(5);
    mainLayout->addWidget(topFiller);
    mainLayout->addLayout(editLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(bottomFiller);

    QLabel *ipLabel = new QLabel(tr("IP address"));
    QLabel *macAddrLabel = new QLabel(tr("MAC address"));
    QLabel *statusLabel = new QLabel(tr("Status"));

    ipEdit = new QLineEdit(tr("192.168.1.200"));
    macEdit = new QLineEdit(tr("00:03:34:"));
    statusEdit = new QLineEdit;

    editLayout->addWidget(ipLabel, 0, 0);
    editLayout->addWidget(ipEdit, 0, 1);
    editLayout->addWidget(macAddrLabel, 1, 0);
    editLayout->addWidget(macEdit, 1, 1);
    editLayout->addWidget(statusLabel, 2, 0);
    editLayout->addWidget(statusEdit, 2, 1);

    QPushButton *startConnBtn = new QPushButton(tr("Connect"));
    //QPushButton *setMac = new QPushButton(tr("Set MAC"));

    buttonLayout->addStretch();
    buttonLayout->addWidget(startConnBtn);
    //buttonLayout->addWidget( setMac );

    widget->setLayout( mainLayout );

    createActions();
    createMenus();

    QString message = tr("Connects with IP addres & then sets mac address");
    statusBar()->showMessage(message);

    connect(startConnBtn,SIGNAL(clicked()),this,SLOT(startConn()));
    connect(&thread,SIGNAL(newStatus(QString)),this,SLOT(showStatus(QString)));
    //connect( &client, SIGNAL(connected()), this, SLOT(displayConnected()) );
    //connect( setMac, SIGNAL(clicked()), this, SLOT(setMac()));
    //connect( &client, SIGNAL(readyRead()), this, SLOT(startRead()) );
    //connect( &client, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError()) );


    startConnBtn->setDefault(true);
    setWindowTitle(tr( "iMac: MAC configurator" ));
    statusEdit->setDisabled(TRUE);
    macEdit->setDisabled(TRUE);
    setMinimumSize(160, 100);
    resize(320, 200);
}

Client::~Client()
{

}

void Client::startConn( void ){
    //QHostAddress addr(ipEdit->text());
    QString addr(ipEdit->text());
    thread.connectSetMac(addr,2002);
}

void Client::showStatus(const QString &nextStatus){
    statusEdit->setDisabled(TRUE);
    macEdit->setEnabled(TRUE);
    statusEdit->setText(nextStatus);
    //cmdState = INITIAL;
}


//void Client::displayError( void ){
//    statusEdit->setDisabled(TRUE);
//    macEdit->setDisabled(TRUE);
//    statusEdit->setText( "Cannot connect" );
//    cmdState = INITIAL;
//    //client.close();
//}

//void Client::setMac( void ){
//    QByteArray macAddr, cmd;
//
//    switch( cmdState ){
//    case SET_MAC:
//        cmd = "s -MC";
//        macAddr.insert( 0, macEdit->text() );
//        client.write( cmd, cmd.length() );
//        client.write( macAddr, macAddr.length() );
//        client.write( "\n", 1 );
//        cmdState = SET_MAC_CMD_OK;
//        break;
//
//    default:
//        break;
//    }
//    return;
//}
//

//void Client::startRead( void ){
//    char buffer[ 1024 ] = { 0 };
//    QString qRespBuff;
//    int retVal = 0;
//    string respStr;
//    QByteArray cmd;
//    QPalette pal;
//    pal.setColor( QPalette::Text, Qt::darkGreen);
//
//    switch( cmdState ){
//    case INITIAL:
//        client.read( buffer, client.bytesAvailable() );
//        qRespBuff.insert( 0, buffer );
//        retVal = qRespBuff.compare( qRespBuff, "Administrator Password?" );
//
//        if( retVal == 0 ){
//            cmdState = PASSWORD_OK;
//            cmd = "00000000\n";
//            client.write( cmd, cmd.length() );
//        }else{
//            cmdState = INITIAL;
//        }
//        break;
//
//    case ENTER_PASSWORD:
//        break;
//
//    case PASSWORD_OK:
//        client.read( buffer, client.bytesAvailable() );
//        qRespBuff.insert( 0, buffer );
//            retVal = qRespBuff.endsWith( "SD>" );
//        if( retVal != string::npos ){
//            cmdState = SET_MAC;
//        }else{
//            cmdState = ENTER_PASSWORD;
//        }
//        break;
//
//    case SET_MAC:
//        break;
//
//    case SET_MAC_CMD_OK:
//        client.read( buffer, client.bytesAvailable() );
//        qRespBuff.insert( 0, buffer );
//            retVal = qRespBuff.startsWith("CMD_OK");
//        if( retVal == TRUE ){
//            cmd = "c\n";
//            client.write( cmd, cmd.length() );
//            cmdState = SET_MAC_PERM_OK;
//        }else{
//            retVal = qRespBuff.startsWith("WRONG_ARG_OR_VALUE" );
//            if( retVal == TRUE ){
//                cmdState = SET_MAC;
//                statusEdit->setPalette( pal );
//                statusEdit->setText( "Error: Chk Mac Addr" );
//            }
//        }
//        break;
//
//     case SET_MAC_PERM:
//        break;
//
//     case SET_MAC_PERM_OK:
//        client.read( buffer, client.bytesAvailable() );
//        qRespBuff.insert( 0, buffer );
//            retVal = qRespBuff.startsWith("SAVE_OK");
//        if( retVal == TRUE ){
//            cmdState = RESET_ST;
//            cmd = "RESET\n";
//            client.write( cmd, cmd.length() );
//        }else{
//            cmdState = SET_MAC_PERM_OK;
//        }
//        break;
//
//     case CLOSE_SOCK:
//        break;
//
//     case RESET_ST:
//        client.read( buffer, client.bytesAvailable() );
//        qRespBuff.insert( 0, buffer );
//        retVal = qRespBuff.startsWith("PLEASE_WAIT_15_SECONDS_FOR_DEVICE_TO_REBOOT");
//        if( retVal == TRUE ){
//            statusEdit->setPalette( pal );
//            statusEdit->setText( "Done,Rebooting.." );
//            client.close();
//            cmdState = INITIAL;
//        }else{
//            cmdState = SET_MAC_PERM_OK;
//        }
//        break;
//    }
//    respStr = qRespBuff.toStdString();
//
//    cout<<respStr<<endl;
//}

void Client::setModelithx()
{
    modelType = ITHX_SD;
}

void Client::setModelibtx()
{
    modelType = IBTX_SD;
}

void Client::about()
{
     //infoLabel->setText(tr("Invoked <b>Help|About</b>"));
     QMessageBox::about(this, tr("About"),
        tr("<center><h2>Mac address configurator</h2><br>"
           "Ajay Sharma<br>"
           "<a href=\"mailto:asharma@newportus.com\">asharma@newportus.com</a></center>"));
}


void Client::createActions( void ){
     exitAct = new QAction(tr("E&xit"), this);
     exitAct->setShortcut(tr("Ctrl+Q"));
     exitAct->setStatusTip(tr("Exit the application"));
     connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

     ithxAct = new QAction(tr("&iTHX-SD"), this);
     ithxAct->setCheckable(true);
     ithxAct->setStatusTip(tr("iTHX-SD"));
     connect(ithxAct, SIGNAL(triggered()), this, SLOT(setModelithx()));

     ibtxAct = new QAction(tr("&iBTX-SD"), this);
     ibtxAct->setCheckable(true);
     ibtxAct->setStatusTip(tr("iBTX-SD"));
     connect(ibtxAct, SIGNAL(triggered()), this, SLOT(setModelibtx()));

     modelsGroup = new QActionGroup(this);
     modelsGroup->addAction(ithxAct);
     modelsGroup->addAction(ibtxAct);
     ithxAct->setChecked(true);

     aboutAct = new QAction(tr("&About"), this);
     aboutAct->setStatusTip(tr("Show the application's About box"));
     connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void Client::createMenus()
 {
     fileMenu = menuBar()->addMenu(tr("&File"));
     fileMenu->addAction(exitAct);

     editMenu = menuBar()->addMenu(tr("&Model"));
     editMenu->addAction(ithxAct);
     editMenu->addAction(ibtxAct);

     helpMenu = menuBar()->addMenu(tr("&Help"));
     helpMenu->addAction(aboutAct);
 }

