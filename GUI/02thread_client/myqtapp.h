#ifndef MYQTAPP_H
#define MYQTAPP_H

#include "ui_myqtapp.h"
#include "mythread.h"



class myQtApp : public QWidget, private Ui::myQtAppDLG
{
	Q_OBJECT

public:
	myQtApp(QWidget *parent = 0);

public slots:
	void launch_in_main();
	void launch_in_new();

private:
	MyThread *thread; // this is our thread

};


#endif
