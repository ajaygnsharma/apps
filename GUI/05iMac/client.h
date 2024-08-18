#ifndef CLIENT_H
#define CLIENT_H

#include <QtGui/QDialog>

#include <QtNetwork>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QMainWindow>

#include <iostream>
#include "macthread.h"

#define ITHX_SD 0
#define IBTX_SD 1

#define MODEL ITHX_SD

class QLineEdit;
class QLabel;
class QAction;
class QActionGroup;
class QMenu;

class Client : public QMainWindow{
    Q_OBJECT

public:
   Client(QWidget *parent = 0);
    ~Client();

public slots:
    void startConn();
    void showStatus(const QString &nextStatus);
    //void displayConnected();
    //void setMac();
    //void startRead();
    //void displayError();
    void setModelithx();
    void setModelibtx();
    void about();


private:
    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *helpMenu;
    QActionGroup *modelsGroup;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *ibtxAct;
    QAction *ithxAct;
    //QAction *eitAct;

    QLineEdit *ipEdit;
    QLineEdit *macEdit;
    QLineEdit *statusEdit;

    //QTcpSocket client;
    //QTimer  *timer;
    QPalette pal;

    int modelType;

    MacThread thread;
    QString nextStatus;
};

#endif // CLIENT_H
