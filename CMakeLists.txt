#-------------------------------------------------
#
# Project created by QtCreator 2014-11-22T16:07:11
#
#-------------------------------------------------
cmake_minimum_required(VERSION 2.7)
PROJECT(quantum_cpp)
SET(CMAKE_CXX_FLAGS "-Wall -std=c++11 -lquantum -fopenmp -DUSE_NOISE -DCR_NOISE -O0 -g")
##SET(CMAKE_INCLUDE_PATH,  ${CMAKE_INCLUDE_PATH}, "/home/lberserq/svn/lq/libquantum-1.1.1/.libs/" ${CMAKE_CURRENT_SOURCE_DIR}/include/)

SET (CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS_INIT} $ENV{LDFLAGS} "-fopenmp -ltinyxml")

SET(CMAKE_INCLUDE_PATH,  ${CMAKE_INCLUDE_PATH}, "/usr/lib64/mpi/gcc/openmpi/include/openmpi/ompi/mpi/cxx/" )
SET(CMAKE_INCLUDE_PATH,  ${CMAKE_INCLUDE_PATH}, "/usr/lib64/mpi/gcc/openmpi/include/" )
SET(CMAKE_INCLUDE_PATH,  ${CMAKE_INCLUDE_PATH}, "./include" )
SET (CMAKE_MODULE_LINKER_FLAGS ${CMAKE_MODULE_LINKER_FLAGS_INIT} $ENV{LDFLAGS} "-fopenmp -ltinyxml")

SET(SOURCE
        ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/common/qmatrix.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/common/adder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/common/common.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/common/measure.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/common/multiplier.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/common/qft.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/common/qreghelper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/gates/mpi/mpi_gatesimpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/gates/omp/omp_gatesimpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/register/shared/sharedqregister.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/register/single/singleqregister.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/common/common_rand.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/common/xmlparsers.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/noise/crauss/crauss_noise.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/noise/crot/crot_noise.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/noise/unitary/unitary_noise.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/serializers/text/textserializer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/serializers/binary/binaryserializer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/serializers/contexthelper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/register/qregisterfactory.cpp
        ##PARENT_SCOPE
        )

SET(HEADERS
    ${HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/commonworld.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/parallelsubsystemcommon.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/qscript_stubs.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/quantum/adder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/quantum/common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/quantum/multiplier.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/quantum/qft.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/quantum/qreghelper.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/config.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/cvariant.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/qmath.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/qmatrix.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/quantum_common_impl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/quantum_common_lib.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/iface/infra/icommonworld.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/iface/quantum/igates.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/iface/quantum/inoise.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/iface/quantum/iqregister.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/iface/quantum/measure.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/gates/mpi/mpi_gatesimpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/gates/omp/omp_gatesimpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/register/shared/sharedqregister.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/register/single/singleqregister.h
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/impls_lists/common_world.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/impls_lists/quantum_gates.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/impls_lists/quantum_noise.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/impls_lists/quantum_register.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/register/registers_all.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/impls_lists/quantum_gates.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/impls_lists/quantum_noise.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/impls_lists/quantum_register.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/register/registers_all.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/noise/noise_all.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/gates/gates_all.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/common_func.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/common_rand.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/quantum_common_circuits.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/xmlparsers.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/noise/crauss/crauss_noise.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/noise/crot/crot_noise.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/noise/unitary/unitary_noise.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/noise/noise_random.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/noise/stubs/stubs_noise.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/noise/simple_noise_provider.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/iface/infra/iregserializer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/iface/infra/iobject.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/serializers/text/textserializer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/serializers/binary/binaryserializer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/regserialization.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/serializers/contexthelper.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/serializers/serializers_common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/quantum/register/qregisterfactory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/io.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/infra/mpiio.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common/utils.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/impls/infra/serializers/serializers_all.h
        ##PARENT_SCOPE
        )

find_package(MPI)


SET(${CMAKE_CXX_LINK_EXECUTABLE} mpicxx)
SET(${CMAKE_C_LINK_EXECUTABLE} mpicc)
SET(${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES} "tinyxml")

ADD_EXECUTABLE(quantum_cpp ${SOURCE} ${HEADERS})
TARGET_LINK_LIBRARIES(quantum_cpp "/home/lberserq/svn/lq/libquantum-1.1.1/.libs/libquantum.a")
TARGET_LINK_LIBRARIES(quantum_cpp ${MPI_C_LIBRARIES})
TARGET_LINK_LIBRARIES(quantum_cpp ${MPI_CXX_LIBRARIES})

TARGET_LINK_LIBRARIES(quantum_cpp tinyxml)

##SET(CMAKE_INCLUDE_PATH,  ${CMAKE_INCLUDE_PATH}, "/home/lberserq/svn/lq/libquantum-1.1.1/.libs/" ${CMAKE_CURRENT_SOURCE_DIR}/include/)

TARGET_INCLUDE_DIRECTORIES(quantum_cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/iface)
TARGET_INCLUDE_DIRECTORIES(quantum_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/common)
TARGET_INCLUDE_DIRECTORIES(quantum_cpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/ "/home/lberserq/svn/lq/libquantum-1.1.1/.libs/")

TARGET_INCLUDE_DIRECTORIES(quantum_cpp PRIVATE ${MPI_INCLUDE_PATH})
