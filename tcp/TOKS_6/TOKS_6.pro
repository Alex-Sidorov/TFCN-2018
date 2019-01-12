#-------------------------------------------------
#
# Project created by QtCreator 2018-11-03T05:38:28
#
#-------------------------------------------------

QT       += core gui
QT	 += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TOKS_6
TEMPLATE = app


SOURCES += main.cpp\
    bit_stuffing.cpp \
    crc.cpp \
    mainwindow.cpp

HEADERS  += \
    bit_stuffing.h \
    crc.h \
    mainwindow.h

FORMS    += \
    mainwindow.ui
