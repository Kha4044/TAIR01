/****************************************************************************
** Meta object code from reading C++ file 'widget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN6WidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto Widget::qt_create_metaobjectdata<qt_meta_tag_ZN6WidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Widget",
        "dataFromVNA",
        "",
        "data",
        "VNAcomand*",
        "cmd",
        "errorMessage",
        "code",
        "message",
        "startScanFromQml",
        "ip",
        "port",
        "startKHz",
        "stopKHz",
        "points",
        "band",
        "stopScanFromQml",
        "applyGraphSettings",
        "QVariantList",
        "graphs",
        "QVariantMap",
        "params",
        "setPowerMeasuringMode",
        "enabled",
        "updateConnectionSettings"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'dataFromVNA'
        QtMocHelpers::SlotData<void(const QString &, VNAcomand *)>(1, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { 0x80000000 | 4, 5 },
        }}),
        // Slot 'errorMessage'
        QtMocHelpers::SlotData<void(int, const QString &)>(6, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 7 }, { QMetaType::QString, 8 },
        }}),
        // Method 'startScanFromQml'
        QtMocHelpers::MethodData<void(const QString &, quint16, int, int, int, int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 }, { QMetaType::UShort, 11 }, { QMetaType::Int, 12 }, { QMetaType::Int, 13 },
            { QMetaType::Int, 14 }, { QMetaType::Int, 15 },
        }}),
        // Method 'stopScanFromQml'
        QtMocHelpers::MethodData<void(const QString &, int)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 }, { QMetaType::Int, 11 },
        }}),
        // Method 'applyGraphSettings'
        QtMocHelpers::MethodData<void(const QVariantList &, const QVariantMap &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 18, 19 }, { 0x80000000 | 20, 21 },
        }}),
        // Method 'setPowerMeasuringMode'
        QtMocHelpers::MethodData<void(bool)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 23 },
        }}),
        // Method 'updateConnectionSettings'
        QtMocHelpers::MethodData<void(const QString &, quint16)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 10 }, { QMetaType::UShort, 11 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Widget, qt_meta_tag_ZN6WidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Widget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6WidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6WidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6WidgetE_t>.metaTypes,
    nullptr
} };

void Widget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Widget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->dataFromVNA((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<VNAcomand*>>(_a[2]))); break;
        case 1: _t->errorMessage((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->startScanFromQml((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[6]))); break;
        case 3: _t->stopScanFromQml((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 4: _t->applyGraphSettings((*reinterpret_cast<std::add_pointer_t<QVariantList>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 5: _t->setPowerMeasuringMode((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 6: _t->updateConnectionSettings((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<quint16>>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObject *Widget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Widget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6WidgetE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Widget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
