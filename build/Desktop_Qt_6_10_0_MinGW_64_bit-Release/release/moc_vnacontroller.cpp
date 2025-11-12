/****************************************************************************
** Meta object code from reading C++ file 'vnacontroller.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../vnacontroller.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vnacontroller.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13VnaControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto VnaController::qt_create_metaobjectdata<qt_meta_tag_ZN13VnaControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "VnaController",
        "sendCommands",
        "",
        "QHostAddress",
        "host",
        "port",
        "QList<VNAcomand*>",
        "commands",
        "traceDataReady",
        "traceNum",
        "QList<qreal>",
        "values",
        "rawVnaResponse",
        "scpiCmd",
        "response",
        "configureAndStart",
        "startKHz",
        "stopKHz",
        "points",
        "bandwidthHz",
        "QList<TraceInfo>",
        "traces",
        "updateTraces",
        "stop",
        "onClientDataReceived",
        "data",
        "VNAcomand*",
        "cmd"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'sendCommands'
        QtMocHelpers::SignalData<void(const QHostAddress &, quint16, QVector<VNAcomand*>)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { QMetaType::UShort, 5 }, { 0x80000000 | 6, 7 },
        }}),
        // Signal 'traceDataReady'
        QtMocHelpers::SignalData<void(int, QVector<qreal>)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 9 }, { 0x80000000 | 10, 11 },
        }}),
        // Signal 'rawVnaResponse'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 14 },
        }}),
        // Slot 'configureAndStart'
        QtMocHelpers::SlotData<void(int, int, int, int, const QVector<TraceInfo> &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 16 }, { QMetaType::Int, 17 }, { QMetaType::Int, 18 }, { QMetaType::Int, 19 },
            { 0x80000000 | 20, 21 },
        }}),
        // Slot 'updateTraces'
        QtMocHelpers::SlotData<void(const QVector<TraceInfo> &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 20, 21 },
        }}),
        // Slot 'stop'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onClientDataReceived'
        QtMocHelpers::SlotData<void(const QString &, VNAcomand *)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 25 }, { 0x80000000 | 26, 27 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<VnaController, qt_meta_tag_ZN13VnaControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject VnaController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13VnaControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13VnaControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13VnaControllerE_t>.metaTypes,
    nullptr
} };

void VnaController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<VnaController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->sendCommands((*reinterpret_cast<std::add_pointer_t<QHostAddress>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QList<VNAcomand*>>>(_a[3]))); break;
        case 1: _t->traceDataReady((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QList<qreal>>>(_a[2]))); break;
        case 2: _t->rawVnaResponse((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 3: _t->configureAndStart((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QList<TraceInfo>>>(_a[5]))); break;
        case 4: _t->updateTraces((*reinterpret_cast<std::add_pointer_t<QList<TraceInfo>>>(_a[1]))); break;
        case 5: _t->stop(); break;
        case 6: _t->onClientDataReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<VNAcomand*>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<qreal> >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (VnaController::*)(const QHostAddress & , quint16 , QVector<VNAcomand*> )>(_a, &VnaController::sendCommands, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (VnaController::*)(int , QVector<qreal> )>(_a, &VnaController::traceDataReady, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (VnaController::*)(const QString & , const QString & )>(_a, &VnaController::rawVnaResponse, 2))
            return;
    }
}

const QMetaObject *VnaController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VnaController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13VnaControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int VnaController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void VnaController::sendCommands(const QHostAddress & _t1, quint16 _t2, QVector<VNAcomand*> _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}

// SIGNAL 1
void VnaController::traceDataReady(int _t1, QVector<qreal> _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void VnaController::rawVnaResponse(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}
QT_WARNING_POP
