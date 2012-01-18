#-------------------------------------------------
#
# Project created by QtCreator 2012-01-15T01:33:43
#
#-------------------------------------------------

QT       += core gui

TARGET = SS_SaveEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gamefile.cpp \
    CRC32.cpp

HEADERS  += mainwindow.h \
    gamefile.h \
    CRC32.h

FORMS    += mainwindow.ui
DEFINES += DEBUG
