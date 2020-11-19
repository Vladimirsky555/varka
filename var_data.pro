#-------------------------------------------------
#
# Project created by QtCreator 2020-09-13T12:36:16
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = var_data
TEMPLATE = app

win32: RC_ICONS = $$PWD/bin/images/app.ico


SOURCES += main.cpp\
        mainwindow.cpp \
    data.cpp \
    dialog.cpp \
    model.cpp \
    application.cpp \
    showitemwindow.cpp \
    regexphighlighter.cpp

HEADERS  += mainwindow.h \
    data.h \
    dialog.h \
    model.h \
    application.h \
    showitemwindow.h \
    regexphighlighter.h

FORMS    += mainwindow.ui \
    dialog.ui \
    showitemwindow.ui

RESOURCES += \
    bin/images.qrc
