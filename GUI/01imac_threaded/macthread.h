#ifndef MACTHREAD_H
#define MACTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#define INITIAL         0
#define ENTER_PASSWORD  1
#define PASSWORD_OK     2
#define SET_MAC 	3
#define SET_MAC_CMD_OK  4
#define SET_MAC_PERM    5
#define SET_MAC_PERM_OK 6
#define CLOSE_SOCK      7
#define RESET_ST        8

class MacThread : public QThread
{
    Q_OBJECT

public:
    MacThread(QObject *parent = 0);
    ~MacThread();

    void connectSetMac(const QString &hostName, quint16 port);
    void run();

signals:
    void newStatus(const QString &fortune);
    void error(int socketError, const QString &message);

private:
    QString hostName;
    quint16 port;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
    //int cmdState;
};

#endif //MACTHREAD_H
