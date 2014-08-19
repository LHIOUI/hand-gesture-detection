#-------------------------------------------------
#
# Project created by QtCreator 2014-08-18T12:21:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gesture
TEMPLATE = app


SOURCES += main.cpp\
        frame.cpp \
    general.cpp \
    RecSample.cpp \
    HandGesture.cpp

HEADERS  += frame.h \
    AccessUnit.h \
    general.h \
    RecSample.h \
    HandGesture.h

FORMS    += frame.ui

INCLUDEPATH += C:\apps\opencv2.4.9\include\

LIBS += -L"C:\apps\opencv2.4.9\x64\mingw\lib"
LIBS += -lopencv_core249.dll
LIBS += -lopencv_highgui249.dll
LIBS += -lopencv_imgproc249.dll
LIBS += -lopencv_legacy249.dll
LIBS += -lopencv_ml249.dll
LIBS += -lopencv_video249.dll
