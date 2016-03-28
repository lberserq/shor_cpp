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
QMAKE_CXXFLAGS += -Werror -std=c++03 -lquantum -fopenmp -DUSE_NOISE -DFULL_NOISE
TEMPLATE = app

QMAKE_INCDIR +=  /home/lberserq/svn/lq/libquantum-1.1.1/.libs/
LIBS += /home/lberserq/svn/lq/libquantum-1.1.1/.libs/libquantum.a
QMAKE_LFLAGS += -fopenmp -ltinyxml

INCLUDEPATH += /usr/lib64/mpi/gcc/openmpi/include/openmpi/ompi/mpi/cxx
INCLUDEPATH += /usr/lib64/mpi/gcc/openmpi/include/
QMAKE_LINK = mpicxx

SOURCES += main.cpp \
    qregister.cpp \
    qmatrix.cpp \
    common.cpp \
    adder.cpp \
    multiplier.cpp \
    qft.cpp \
    measure.cpp \
    tests.cpp \
    xml_parsers.cpp \
    gatesimpl.cpp \
    noise.cpp

HEADERS += \
    qregister.h \
    qmatrix.h \
    common.h \
    adder.h \
    config.h \
    multiplier.h \
    qft.h \
    measure.h \
    tests.h \
    xml_parsers.h \
    common_lib.h \
    igates.h \
    gatesimpl.h \
    noise.h \
    inoise.h \
    commonworld.h \
    icommonworld.h \
    common_impl.h \
    qmath.h \
    qscript_stubs.h \
    cvariant.h
