QT += core gui widgets quick quickwidgets qml charts network charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \

    createrchart.cpp \
    main.cpp \
    socket.cpp \
    vnacomand.cpp \
    widget.cpp

HEADERS += \

    createrchart.h \
    socket.h \
    vnaclient.h \
    vnacomand.h \
    widget.h


FORMS += \
    widget.ui

RESOURCES += \
    res.qrc

# Default rules for deployment
