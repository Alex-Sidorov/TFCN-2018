#-------------------------------------------------
#
# Project created by QtCreator 2018-10-23T12:41:36
#
#-------------------------------------------------

QT       += core gui
QT       +=serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TOKS_5
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bit_stuffing.cpp \
    crc.cpp \
    port.cpp

HEADERS  += mainwindow.h \
    bit_stuffing.h \
    crc.h \
    port.h

FORMS += \
    mainwindow.ui
