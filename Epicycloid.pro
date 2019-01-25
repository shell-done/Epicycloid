QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Epicycloid
TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    window.cpp \
    eimage.cpp \
    animation.cpp \
    function.cpp

HEADERS += \
    window.h \
    eimage.h \
    animation.h \
    function.h
