#-------------------------------------------------
#
# Project created by QtCreator 2015-02-07T20:16:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = handCursor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gklibs.cpp \
    handprocess.cpp \
    objectcontrol.cpp \
    mlibs.cpp

HEADERS  += mainwindow.h \
    gklibs.h \
    handprocess.h \
    mlibs.h \
    objectcontrol.h

FORMS    += mainwindow.ui

LIBS += /usr/lib/libopencv_core.so.2.4\
        /usr/lib/libopencv_video.so.2.4\
        /usr/lib/libopencv_highgui.so.2.4\
        /usr/lib/libopencv_videostab.so.2.4\
        /usr/lib/libopencv_ts.so.2.4\
        /usr/lib/libopencv_imgproc.so.2.4\
        /usr/lib/libopencv_contrib.so.2.4
