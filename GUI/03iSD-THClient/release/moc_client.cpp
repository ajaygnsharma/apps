/****************************************************************************
** Meta object code from reading C++ file 'client.h'
**
** Created: Mon Mar 29 16:04:36 2010
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../client.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'client.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Client[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x0a,
      23,    7,    7,    7, 0x0a,
      39,    7,    7,    7, 0x0a,
      51,    7,    7,    7, 0x0a,
      70,    7,    7,    7, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Client[] = {
    "Client\0\0startPolling()\0startTransfer()\0"
    "startRead()\0displayConnected()\0"
    "stopPolling()\0"
};

const QMetaObject Client::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Client,
      qt_meta_data_Client, 0 }
};

const QMetaObject *Client::metaObject() const
{
    return &staticMetaObject;
}

void *Client::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Client))
        return static_cast<void*>(const_cast< Client*>(this));
    return QDialog::qt_metacast(_clname);
}

int Client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: startPolling(); break;
        case 1: startTransfer(); break;
        case 2: startRead(); break;
        case 3: displayConnected(); break;
        case 4: stopPolling(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
