#include <QtGui> 
#include "myqtapp.h"

myQtApp::myQtApp(QWidget *parent)
{
	setupUi(this); // this sets up GUI

	// signals/slots mechanism in action
	connect( pushButton_lmain, SIGNAL( clicked() ), this, SLOT( launch_in_main() ) ); 
	connect( pushButton_lnew, SIGNAL( clicked() ), this, SLOT( launch_in_new() ) ); 
//	connect( pushButton_stopnew, SIGNAL( clicked() ), this, SLOT( stop_in_new() ) ); 

	// enabling/disabling buttons
	//pushButton_stopnew->setEnabled( false );
}


void myQtApp::launch_in_main()
{
	qDebug() << "Executing in main thread, GUI is now blocked";
		
	for(int i=0;i<10;i++)
	{
		qDebug() << "Time to wait: " << 10-i;
		int t=1;

		// some OS specific stuff
		// mingw (3.4.2) sleep on windows is called _sleep and uses microseconds
		#ifdef Q_OS_WIN32
		t = t * 1000;
		_sleep(t);
		#else
		sleep(t);
		#endif

	}

	qDebug() << "Now you can operate with GUI";
}


void myQtApp::launch_in_new()
{
	// create new thread (on heap) and start it
	thread = new MyThread(this);
	thread->start(); // after this, thread's run() method starts 

}


