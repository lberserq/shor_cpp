#-------------------------------------------------
#
# Project created by QtCreator 2014-11-22T16:07:11
#
#-------------------------------------------------

QT       -= core

QT       -= gui

TARGET = quantum_cpp
CONFIG   += console
CONFIG   -= app_bundle
QMAKE_CXXFLAGS += -std=c++11
TEMPLATE = app

QMAKE_INCDIR += /home/lberserq/tmp/libq/libquantum-1.1.1/.libs
LIBS += /home/lberserq/tmp/libq/libquantum-1.1.1/.libs/libquantum.a
QMAKE_LFLAGS += -fopenmp


SOURCES += main.cpp \
    qregister.cpp \
    qmatrix.cpp \
    common.cpp \
    adder.cpp \
    multiplier.cpp \
    qft.cpp \
    measure.cpp \
    tests.cpp

HEADERS += \
    qregister.h \
    qmatrix.h \
    common.h \
    adder.h \
    config.h \
    multiplier.h \
    qft.h \
    measure.h \
    tests.h
