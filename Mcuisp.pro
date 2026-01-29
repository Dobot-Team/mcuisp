TEMPLATE = app

QT       += core gui qml quick serialport

TARGET = Mcuisp


CONFIG += c++11

SOURCES += main.cpp \
    Qapi.cpp \
    Serialhelper.cpp \
    Mcuisp.cpp \
    hex2bin.cpp

HEADERS  += \
    Qapi.h \
    Utils.h \
    Serialhelper.h \
    Mcuisp.h \
    Common.h \
    hex2bin.h

RESOURCES += stocqt.qrc \
    mcuisp.qrc

RC_ICONS = Dobot.ico

unix {
    ICON = Dobot_128.icns
}

DESTDIR = ../McuispOutput

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

