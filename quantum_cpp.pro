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
QMAKE_CXX = g++
QMAKE_CXXFLAGS += -Wall -std=c++98 -lquantum -fopenmp -DUSE_NOISE -DCR_NOISE -O0 -g
TEMPLATE = app

QMAKE_INCDIR +=  /home/lberserq/svn/lq/libquantum-1.1.1/.libs/
LIBS += /home/lberserq/svn/lq/libquantum-1.1.1/.libs/libquantum.a
QMAKE_LFLAGS += -fopenmp -ltinyxml -lpapi -lrt

INCLUDEPATH += /usr/lib64/mpi/gcc/openmpi/include/openmpi/ompi/mpi/cxx
INCLUDEPATH += /usr/lib64/mpi/gcc/openmpi/include/
INCLUDEPATH += ./include

QMAKE_LINK = mpicxx

SOURCES += main.cpp \
    src/impls/infra/common/qmatrix.cpp \
    src/impls/quantum/common/adder.cpp \
    src/impls/quantum/common/common.cpp \
    src/impls/quantum/common/measure.cpp \
    src/impls/quantum/common/multiplier.cpp \
    src/impls/quantum/common/qft.cpp \
    src/impls/quantum/common/qreghelper.cpp \
    src/impls/quantum/gates/mpi/mpi_gatesimpl.cpp \
    src/impls/quantum/gates/omp/omp_gatesimpl.cpp \
    src/impls/quantum/register/shared/sharedqregister.cpp \
    src/impls/quantum/register/single/singleqregister.cpp \
    tests/tests.cpp \
    src/impls/infra/common/common_rand.cpp \
    src/impls/infra/common/xmlparsers.cpp \
    src/impls/quantum/noise/crauss/crauss_noise.cpp \
    src/impls/quantum/noise/crot/crot_noise.cpp \
    src/impls/quantum/noise/unitary/unitary_noise.cpp \
    src/impls/infra/serializers/text/textserializer.cpp \
    src/impls/infra/serializers/binary/binaryserializer.cpp \
    src/impls/infra/serializers/contexthelper.cpp \
    src/impls/quantum/register/qregisterfactory.cpp \
    src/impls/infra/perfcounters/papi/papiperfcounter.cpp \
    src/impls/infra/perfcounters/simple/simpleperfcounter.cpp

HEADERS += \
    include/common/infra/commonworld.h \
    include/common/infra/parallelsubsystemcommon.h \
    include/common/infra/qscript_stubs.h \
    include/common/quantum/adder.h \
    include/common/quantum/common.h \
    include/common/quantum/multiplier.h \
    include/common/quantum/qft.h \
    include/common/quantum/qreghelper.h \
    include/common/config.h \
    include/common/cvariant.h \
    include/common/qmath.h \
    include/common/qmatrix.h \
    include/common/quantum_common_impl.h \
    include/common/quantum_common_lib.h \
    include/iface/infra/icommonworld.h \
    include/iface/quantum/igates.h \
    include/iface/quantum/inoise.h \
    include/iface/quantum/iqregister.h \
    include/iface/quantum/measure.h \
    src/impls/quantum/gates/mpi/mpi_gatesimpl.h \
    src/impls/quantum/gates/omp/omp_gatesimpl.h \
    src/impls/quantum/noise/noise.h \
    src/impls/quantum/register/shared/sharedqregister.h \
    src/impls/quantum/register/single/singleqregister.h \
    tests/tests.h \
    include/common/infra/impls_lists/common_world.h \
    include/common/infra/impls_lists/quantum_gates.h \
    include/common/infra/impls_lists/quantum_noise.h \
    include/common/infra/impls_lists/quantum_register.h \
    src/impls/quantum/register/registers_all.h \
    src/impls/quantum/noise/noise_all.h \
    src/impls/quantum/gates/gates_all.h \
    include/common/infra/common_func.h \
    include/common/infra/common_rand.h \
    include/common/quantum_common_circuits.h \
    include/common/infra/xmlparsers.h \
    src/impls/quantum/noise/crauss/crauss_noise.h \
    src/impls/quantum/noise/crot/crot_noise.h \
    src/impls/quantum/noise/unitary/unitary_noise.h \
    src/impls/quantum/noise/noise_random.h \
    src/impls/quantum/noise/stubs/stubs_noise.h \
    src/impls/quantum/noise/simple_noise_provider.h \
    include/iface/infra/iregserializer.h \
    include/iface/infra/iobject.h \
    src/impls/infra/serializers/text/textserializer.h \
    src/impls/infra/serializers/binary/binaryserializer.h \
    include/common/infra/regserialization.h \
    src/impls/infra/serializers/contexthelper.h \
    src/impls/infra/serializers/serializers_common.h \
    src/impls/quantum/register/qregisterfactory.h \
    include/common/infra/io.h \
    include/common/infra/mpiio.h \
    include/common/utils.h \
    src/impls/infra/serializers/serializers_all.h \
    include/iface/infra/iperfcounter.h \
    src/impls/infra/perfcounters/papi/papiperfcounter.h \
    src/impls/infra/perfcounters/simple/simpleperfcounter.h \
    src/impls/infra/perfcounters/stub/stubperfcounter.h \
    src/impls/infra/perfcounters/perfcounters_all.h \
    include/common/infra/impls_lists/infra_serializer.h \
    include/common/infra/impls_lists/infra_perfcounter.h

DISTFILES += \
    profiler.py
