/****************************************************************************
** Meta object code from reading C++ file 'uart.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Solar__Pro_GUI/uart.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'uart.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Uart_t {
    QByteArrayData data[14];
    char stringdata0[114];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Uart_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Uart_t qt_meta_stringdata_Uart = {
    {
QT_MOC_LITERAL(0, 0, 4), // "Uart"
QT_MOC_LITERAL(1, 5, 13), // "debugReceived"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 3), // "str"
QT_MOC_LITERAL(4, 24, 14), // "packetReceived"
QT_MOC_LITERAL(5, 39, 4), // "data"
QT_MOC_LITERAL(6, 44, 4), // "open"
QT_MOC_LITERAL(7, 49, 4), // "path"
QT_MOC_LITERAL(8, 54, 5), // "close"
QT_MOC_LITERAL(9, 60, 4), // "send"
QT_MOC_LITERAL(10, 65, 8), // "getPorts"
QT_MOC_LITERAL(11, 74, 19), // "QList<QextPortInfo>"
QT_MOC_LITERAL(12, 94, 11), // "getUSBPorts"
QT_MOC_LITERAL(13, 106, 7) // "receive"

    },
    "Uart\0debugReceived\0\0str\0packetReceived\0"
    "data\0open\0path\0close\0send\0getPorts\0"
    "QList<QextPortInfo>\0getUSBPorts\0receive"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Uart[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   60,    2, 0x0a /* Public */,
       8,    0,   63,    2, 0x0a /* Public */,
       9,    1,   64,    2, 0x0a /* Public */,
      10,    0,   67,    2, 0x0a /* Public */,
      12,    0,   68,    2, 0x0a /* Public */,
      13,    0,   69,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QByteArray,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    5,
    0x80000000 | 11,
    0x80000000 | 11,
    QMetaType::Void,

       0        // eod
};

void Uart::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Uart *_t = static_cast<Uart *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->debugReceived((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->packetReceived((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 2: _t->open((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->close(); break;
        case 4: _t->send((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 5: { QList<QextPortInfo> _r = _t->getPorts();
            if (_a[0]) *reinterpret_cast< QList<QextPortInfo>*>(_a[0]) = std::move(_r); }  break;
        case 6: { QList<QextPortInfo> _r = _t->getUSBPorts();
            if (_a[0]) *reinterpret_cast< QList<QextPortInfo>*>(_a[0]) = std::move(_r); }  break;
        case 7: _t->receive(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (Uart::*_t)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Uart::debugReceived)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Uart::*_t)(QByteArray );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Uart::packetReceived)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject Uart::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Uart.data,
      qt_meta_data_Uart,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Uart::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Uart::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Uart.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Uart::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Uart::debugReceived(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Uart::packetReceived(QByteArray _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
