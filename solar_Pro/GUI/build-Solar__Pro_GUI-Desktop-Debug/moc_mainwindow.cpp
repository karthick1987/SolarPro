/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Solar__Pro_GUI/mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[16];
    char stringdata0[323];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 27), // "on_pushButton_close_clicked"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 26), // "on_pushButton_open_clicked"
QT_MOC_LITERAL(4, 67, 29), // "on_button_NetDiscover_clicked"
QT_MOC_LITERAL(5, 97, 27), // "on_button_Emergency_clicked"
QT_MOC_LITERAL(6, 125, 43), // "on_pushButton_SetWindSpeedThr..."
QT_MOC_LITERAL(7, 169, 36), // "on_pushButton_SetManualAngle_..."
QT_MOC_LITERAL(8, 206, 7), // "receive"
QT_MOC_LITERAL(9, 214, 3), // "str"
QT_MOC_LITERAL(10, 218, 4), // "send"
QT_MOC_LITERAL(11, 223, 4), // "data"
QT_MOC_LITERAL(12, 228, 15), // "packet_received"
QT_MOC_LITERAL(13, 244, 32), // "on_radioButton_AngleAuto_clicked"
QT_MOC_LITERAL(14, 277, 34), // "on_radioButton_AngleManual_cl..."
QT_MOC_LITERAL(15, 312, 10) // "systemTime"

    },
    "MainWindow\0on_pushButton_close_clicked\0"
    "\0on_pushButton_open_clicked\0"
    "on_button_NetDiscover_clicked\0"
    "on_button_Emergency_clicked\0"
    "on_pushButton_SetWindSpeedThreshold_clicked\0"
    "on_pushButton_SetManualAngle_clicked\0"
    "receive\0str\0send\0data\0packet_received\0"
    "on_radioButton_AngleAuto_clicked\0"
    "on_radioButton_AngleManual_clicked\0"
    "systemTime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x08 /* Private */,
       3,    0,   75,    2, 0x08 /* Private */,
       4,    0,   76,    2, 0x08 /* Private */,
       5,    0,   77,    2, 0x08 /* Private */,
       6,    0,   78,    2, 0x08 /* Private */,
       7,    0,   79,    2, 0x08 /* Private */,
       8,    1,   80,    2, 0x08 /* Private */,
      10,    1,   83,    2, 0x08 /* Private */,
      12,    1,   86,    2, 0x08 /* Private */,
      13,    0,   89,    2, 0x08 /* Private */,
      14,    0,   90,    2, 0x08 /* Private */,
      15,    0,   91,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void, QMetaType::QByteArray,   11,
    QMetaType::Void, QMetaType::QByteArray,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_pushButton_close_clicked(); break;
        case 1: _t->on_pushButton_open_clicked(); break;
        case 2: _t->on_button_NetDiscover_clicked(); break;
        case 3: _t->on_button_Emergency_clicked(); break;
        case 4: _t->on_pushButton_SetWindSpeedThreshold_clicked(); break;
        case 5: _t->on_pushButton_SetManualAngle_clicked(); break;
        case 6: _t->receive((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 7: _t->send((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 8: _t->packet_received((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 9: _t->on_radioButton_AngleAuto_clicked(); break;
        case 10: _t->on_radioButton_AngleManual_clicked(); break;
        case 11: _t->systemTime(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
