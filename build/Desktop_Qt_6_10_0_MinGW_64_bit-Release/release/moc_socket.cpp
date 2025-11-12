/****************************************************************************
** Meta object code from reading C++ file 'socket.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../socket.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'socket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN6SocketE_t {};
} // unnamed namespace

template <> constexpr inline auto Socket::qt_create_metaobjectdata<qt_meta_tag_ZN6SocketE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Socket",
        "startScan",
        "",
        "startKHz",
        "stopKHz",
        "points",
        "band",
        "stopScan",
        "sendCommand",
        "QHostAddress",
        "host",
        "port",
        "QList<VNAcomand*>",
        "commands",
        "setGraphSettings",
        "graphCount",
        "QList<int>",
        "traceNumbers",
        "requestFDAT",
        "startPowerMeasurement",
        "stopPowerMeasurement",
        "isPowerMeasuring",
        "onConnected",
        "onDisconnected"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'startScan'
        QtMocHelpers::SlotData<void(int, int, int, int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Int, 4 }, { QMetaType::Int, 5 }, { QMetaType::Int, 6 },
        }}),
        // Slot 'stopScan'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'sendCommand'
        QtMocHelpers::SlotData<void(const QHostAddress &, quint16, const QVector<VNAcomand*> &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 }, { QMetaType::UShort, 11 }, { 0x80000000 | 12, 13 },
        }}),
        // Slot 'setGraphSettings'
        QtMocHelpers::SlotData<void(int, const QVector<int> &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 15 }, { 0x80000000 | 16, 17 },
        }}),
        // Slot 'requestFDAT'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'startPowerMeasurement'
        QtMocHelpers::SlotData<void(int, int, int, int)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 }, { QMetaType::Int, 4 }, { QMetaType::Int, 5 }, { QMetaType::Int, 6 },
        }}),
        // Slot 'stopPowerMeasurement'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'isPowerMeasuring'
        QtMocHelpers::SlotData<bool() const>(21, 2, QMC::AccessPublic, QMetaType::Bool),
        // Slot 'onConnected'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDisconnected'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Socket, qt_meta_tag_ZN6SocketE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Socket::staticMetaObject = { {
    QMetaObject::SuperData::link<VNAclient::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6SocketE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6SocketE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6SocketE_t>.metaTypes,
    nullptr
} };

void Socket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Socket *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->startScan((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4]))); break;
        case 1: _t->stopScan(); break;
        case 2: _t->sendCommand((*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QList<VNAcomand*>>>(_a[3]))); break;
        case 3: _t->setGraphSettings((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QList<int>>>(_a[2]))); break;
        case 4: _t->requestFDAT(); break;
        case 5: _t->startPowerMeasurement((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4]))); break;
        case 6: _t->stopPowerMeasurement(); break;
        case 7: { bool _r = _t->isPowerMeasuring();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 8: _t->onConnected(); break;
        case 9: _t->onDisconnected(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        }
    }
}

const QMetaObject *Socket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Socket::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6SocketE_t>.strings))
        return static_cast<void*>(this);
    return VNAclient::qt_metacast(_clname);
}

int Socket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = VNAclient::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
