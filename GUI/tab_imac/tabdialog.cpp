/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial Usage
 ** Licensees holding valid Qt Commercial licenses may use this file in
 ** accordance with the Qt Commercial License Agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Nokia.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights.  These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 ** If you have questions regarding the use of this file, please contact
 ** Nokia at qt-info@nokia.com.
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

 #include <QtGui>

 #include "tabdialog.h"

 TabDialog::TabDialog(const QString &fileName, QWidget *parent)
     : QDialog(parent)
 {
     QFileInfo fileInfo(fileName);

     tabWidget = new QTabWidget;
     tabWidget->addTab(new GeneralTab(fileInfo), tr("Mac Address"));
     tabWidget->addTab(new PermissionsTab(fileInfo), tr("Readings"));
     tabWidget->addTab(new ApplicationsTab(fileInfo), tr("Applications"));

     buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);

     connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
     connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

     QVBoxLayout *mainLayout = new QVBoxLayout;
     mainLayout->addWidget(tabWidget);
     mainLayout->addWidget(buttonBox);
     setLayout(mainLayout);

     setWindowTitle(tr("32-bit Products Diagnostics Tool"));
 }

 GeneralTab::GeneralTab(const QFileInfo &fileInfo, QWidget *parent)
     : QWidget(parent)
 {
     QLabel *ipAddrLabel = new QLabel(tr("IP Address:"));
     QLineEdit *ipAddrEdit = new QLineEdit("192.168.1.200");

     QLabel *macAddrLabel = new QLabel(tr("Mac Address:"));
     QLineEdit *macAddrEdit = new QLineEdit("00:03:34:");
     //macAddrLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

     QLabel *statusLabel = new QLabel(tr("Status:"));
     //qlonglong size = fileInfo.size()/1024;
     QLabel *statusStrLabel = new QLabel();
     statusStrLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

//     QLabel *lastReadLabel = new QLabel(tr("Last Read:"));
//     QLabel *lastReadValueLabel = new QLabel(fileInfo.lastRead().toString());
//     lastReadValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
//
//     QLabel *lastModLabel = new QLabel(tr("Last Modified:"));
//     QLabel *lastModValueLabel = new QLabel(fileInfo.lastModified().toString());
//     lastModValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

     QVBoxLayout *mainLayout = new QVBoxLayout;
     mainLayout->addWidget(ipAddrLabel);
     mainLayout->addWidget(ipAddrEdit);
     mainLayout->addWidget(macAddrLabel);
     mainLayout->addWidget(macAddrEdit);
     mainLayout->addWidget(statusLabel);
     mainLayout->addWidget(statusStrLabel);
     //mainLayout->addWidget(lastReadLabel);
     //mainLayout->addWidget(lastReadValueLabel);
     //mainLayout->addWidget(lastModLabel);
     //mainLayout->addWidget(lastModValueLabel);
     mainLayout->addStretch(1);
     setLayout(mainLayout);
 }

 PermissionsTab::PermissionsTab(const QFileInfo &fileInfo, QWidget *parent)
     : QWidget(parent)
 {
     QGroupBox *permissionsGroup = new QGroupBox(tr("Readings"));

     QCheckBox *enableRdngChkbox = new QCheckBox(tr("Enable Readings"));
     //if (fileInfo.isReadable())
     //    readable->setChecked(true);

//     QCheckBox *writable = new QCheckBox(tr("Writable"));
//     if ( fileInfo.isWritable() )
//         writable->setChecked(true);
//
//     QCheckBox *executable = new QCheckBox(tr("Executable"));
//     if ( fileInfo.isExecutable() )
//         executable->setChecked(true);

     QGroupBox *ownerGroup = new QGroupBox(tr("Ownership"));

     QLabel *ownerLabel = new QLabel(tr("Owner"));
     QLabel *ownerValueLabel = new QLabel(fileInfo.owner());
     ownerValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

     QLabel *groupLabel = new QLabel(tr("Group"));
     QLabel *groupValueLabel = new QLabel(fileInfo.group());
     groupValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

     QVBoxLayout *permissionsLayout = new QVBoxLayout;
     permissionsLayout->addWidget(enableRdngChkbox);
     //permissionsLayout->addWidget(writable);
     //permissionsLayout->addWidget(executable);
     permissionsGroup->setLayout(permissionsLayout);

     QVBoxLayout *ownerLayout = new QVBoxLayout;
     ownerLayout->addWidget(ownerLabel);
     ownerLayout->addWidget(ownerValueLabel);
     ownerLayout->addWidget(groupLabel);
     ownerLayout->addWidget(groupValueLabel);
     ownerGroup->setLayout(ownerLayout);

     QVBoxLayout *mainLayout = new QVBoxLayout;
     mainLayout->addWidget(permissionsGroup);
     mainLayout->addWidget(ownerGroup);
     mainLayout->addStretch(1);
     setLayout(mainLayout);
 }

 ApplicationsTab::ApplicationsTab(const QFileInfo &fileInfo, QWidget *parent)
     : QWidget(parent)
 {
     QLabel *topLabel = new QLabel(tr("Open with:"));

     QListWidget *applicationsListBox = new QListWidget;
     QStringList applications;

     for (int i = 1; i <= 30; ++i)
         applications.append(tr("Application %1").arg(i));
     applicationsListBox->insertItems(0, applications);

     QCheckBox *alwaysCheckBox;

     if (fileInfo.suffix().isEmpty())
         alwaysCheckBox = new QCheckBox(tr("Always use this application to "
             "open this type of file"));
     else
         alwaysCheckBox = new QCheckBox(tr("Always use this application to "
             "open files with the extension '%1'").arg(fileInfo.suffix()));

     QVBoxLayout *layout = new QVBoxLayout;
     layout->addWidget(topLabel);
     layout->addWidget(applicationsListBox);
     layout->addWidget(alwaysCheckBox);
     setLayout(layout);
 }
