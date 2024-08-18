/********************************************************************************
** Form generated from reading UI file 'myqtapp.ui'
**
** Created: Thu Dec 9 22:46:07 2010
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYQTAPP_H
#define UI_MYQTAPP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_myQtAppDLG
{
public:
    QGroupBox *groupBox;
    QPushButton *pushButton_lmain;
    QFrame *line;
    QPushButton *pushButton_lnew;
    QCheckBox *checkBox;
    QTextEdit *textEdit;

    void setupUi(QWidget *myQtAppDLG)
    {
        if (myQtAppDLG->objectName().isEmpty())
            myQtAppDLG->setObjectName(QString::fromUtf8("myQtAppDLG"));
        myQtAppDLG->resize(372, 347);
        groupBox = new QGroupBox(myQtAppDLG);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 230, 351, 111));
        pushButton_lmain = new QPushButton(groupBox);
        pushButton_lmain->setObjectName(QString::fromUtf8("pushButton_lmain"));
        pushButton_lmain->setGeometry(QRect(20, 20, 211, 24));
        line = new QFrame(groupBox);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(20, 50, 301, 16));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        pushButton_lnew = new QPushButton(groupBox);
        pushButton_lnew->setObjectName(QString::fromUtf8("pushButton_lnew"));
        pushButton_lnew->setGeometry(QRect(20, 70, 211, 24));
        checkBox = new QCheckBox(myQtAppDLG);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(10, 200, 351, 19));
        textEdit = new QTextEdit(myQtAppDLG);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(10, 10, 351, 181));

        retranslateUi(myQtAppDLG);

        QMetaObject::connectSlotsByName(myQtAppDLG);
    } // setupUi

    void retranslateUi(QWidget *myQtAppDLG)
    {
        myQtAppDLG->setWindowTitle(QApplication::translate("myQtAppDLG", "My first Qt GUI App with threads", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("myQtAppDLG", " Thread launcher", 0, QApplication::UnicodeUTF8));
        pushButton_lmain->setText(QApplication::translate("myQtAppDLG", "Launch heavy computing in main thread", 0, QApplication::UnicodeUTF8));
        pushButton_lnew->setText(QApplication::translate("myQtAppDLG", "Launch heavy computing in new thread", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QApplication::translate("myQtAppDLG", "try to check/uncheck me during computing", 0, QApplication::UnicodeUTF8));
        textEdit->setHtml(QApplication::translate("myQtAppDLG", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:MS Shell Dlg 2; font-size:8.25pt; font-weight:400; font-style:normal; text-decoration:none;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:MS; font-size:8pt;\">This little application show basic use of Qt threads.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:MS; font-size:8pt;\">First launch heavy comupting in main (current) thread and try to edit this text. It is not possile, GUI interface is freezed.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:MS; font-size:8pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0"
                        "px; -qt-block-indent:0; text-indent:0px; font-family:MS; font-size:8pt;\">If you launch computing in new thread, GUI is not freezed, you can move/operate with it. </p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:MS; font-size:8pt;\"></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:MS; font-size:8pt;\">Heavy computing is simulated with sleep.</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:MS; font-size:8pt;\">Watch console ouput.</p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:MS; font-size:8pt;\"></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class myQtAppDLG: public Ui_myQtAppDLG {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYQTAPP_H
