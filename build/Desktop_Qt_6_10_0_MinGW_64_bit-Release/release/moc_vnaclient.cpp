/****************************************************************************
** Meta object code from reading C++ file 'vnaclient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../vnaclient.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vnaclient.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9VNAclientE_t {};
} // unnamed namespace

template <> constexpr inline auto VNAclient::qt_create_metaobjectdata<qt_meta_tag_ZN9VNAclientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "VNAclient",
        "connected",
        "",
        "disconnected",
        "error",
        "errorCode",
        "message",
        "dataFromVNA",
        "data",
        "VNAcomand*",
        "cmd",
        "startScan",
        "ip",
        "port",
        "startKHz",
        "stopKHz",
        "points",
        "band",
        "powerDbM",
        "powerFreqKHz",
        "stopScan",
        "sendCommand",
        "QHostAddress",
        "host",
        "QList<VNAcomand*>",
        "commands",
        "setGraphSettings",
        "graphCount",
        "QList<int>",
        "traceNumbers"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'disconnected'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'error'
        QtMocHelpers::SignalData<void(int, const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 }, { QMetaType::QString, 6 },
        }}),
        // Signal 'dataFromVNA'
        QtMocHelpers::SignalData<void(const QString &, VNAcomand *)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { 0x80000000 | 9, 10 },
        }}),
        // Slot 'startScan'
        QtMocHelpers::SlotData<void(const QString &, quint16, int, int, int, int, double, int)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::UShort, 13 }, { QMetaType::Int, 14 }, { QMetaType::Int, 15 },
            { QMetaType::Int, 16 }, { QMetaType::Int, 17 }, { QMetaType::Double, 18 }, { QMetaType::Int, 19 },
        }}),
        // Slot 'stopScan'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'sendCommand'
        QtMocHelpers::SlotData<void(const QHostAddress &, quint16, const QVector<VNAcomand*> &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 22, 23 }, { QMetaType::UShort, 13 }, { 0x80000000 | 24, 25 },
        }}),
        // Slot 'setGraphSettings'
        QtMocHelpers::SlotData<void(int, const QVector<int> &)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 27 }, { 0x80000000 | 28, 29 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<VNAclient, qt_meta_tag_ZN9VNAclientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject VNAclient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9VNAclientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9VNAclientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9VNAclientE_t>.metaTypes,
    nullptr
} };

void VNAclient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<VNAclient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->error((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->dataFromVNA((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<VNAcomand*>>(_a[2]))); break;
        case 4: _t->startScan((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[6])),(*reinterpret_cast<std::add_pointer_t<double>>(_a[7])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[8]))); break;
        case 5: _t->stopScan(); break;
        case 6: _t->sendCommand((*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QList<VNAcomand*>>>(_a[3]))); break;
        case 7: _t->setGraphSettings((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QList<int>>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (VNAclient::*)()>(_a, &VNAclient::connected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (VNAclient::*)()>(_a, &VNAclient::disconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (VNAclient::*)(int , const QString & )>(_a, &VNAclient::error, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (VNAclient::*)(const QString & , VNAcomand * )>(_a, &VNAclient::dataFromVNA, 3))
            return;
    }
}

const QMetaObject *VNAclient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VNAclient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9VNAclientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int VNAclient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void VNAclient::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void VNAclient::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void VNAclient::error(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}

// SIGNAL 3
void VNAclient::dataFromVNA(const QString & _t1, VNAcomand * _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}
QT_WARNING_POP
