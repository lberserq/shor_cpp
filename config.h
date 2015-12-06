#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>
enum
    {
    REG_NUM  = 4
    };
const double g_eps = 1e-8;
typedef unsigned long long state;
#include <complex>
#include <iterator>
typedef std::complex<double> mcomplex;
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <mpi.h>

#define EKA_DECLARE_IID(x)\
    int __eka__id = x;\

#ifdef _MSC_VER
typedef  unsigned int result_t;
#else
typedef  unsigned result_t;
#endif

#include <stdexcept>

namespace ParallelSubSystemHelper {
    struct mpicfg {
        int rank;
        int size;
        mpicfg(int rank = 0, int size = 1) : rank(rank), size(size){}
    };

    mpicfg getConfig();
    /*void AllreduceHelper(long double &val, MPI_Op op) {
        long double gval = 0;
        if (getConfig().size) {
            MPI_Allreduce(&gval, &val, 1, MPI_LONG_DOUBLE, op, MPI_COMM_WORLD);
        }
        val = gval;
    }*/
}
#define dumpvec(vec, Rank) \
{\
    MPI_Barrier(MPI_COMM_WORLD);\
    if (ParallelSubSystemHelper::getConfig().rank == Rank) {\
    std::cerr << "Dumping Vec "  << std::endl; \
    for (size_t i = 0; i < vec.size(); i++) \
    if (i != vec.size() - 1) std::cerr << vec[i] << " "; \
    else std::cerr << vec[i] << "\n"; \
    std::cerr << std::flush; \
    }\
    MPI_Barrier(MPI_COMM_WORLD);\
    }

#define dumpVar(var, Rank) \
{\
    MPI_Barrier(MPI_COMM_WORLD);\
    if (ParallelSubSystemHelper::getConfig().rank == Rank) {\
    std::cerr << "DUMPING VAR == " << var << std::flush << std::endl;\
    }\
    MPI_Barrier(MPI_COMM_WORLD);\
    }

#define dumparray(array, len, Rank) \
{\
    MPI_Barrier(MPI_COMM_WORLD);\
    if (ParallelSubSystemHelper::getConfig().rank == Rank) {\
    std::cerr << "Dumping Array "  << std::endl; \
    for (size_t i = 0; i < len; i++) \
    if (i != len - 1) std::cerr << array[i] << " "; \
    else std::cerr << array[i] << "\n"; \
    std::cerr << std::flush; \
    }\
    MPI_Barrier(MPI_COMM_WORLD);\
    }


#define reduce_helper

extern void userInit();
class StaticQRegister;
class SharedQSimpleQRegister;
typedef SharedQSimpleQRegister UserDefQRegister;
#endif // CONFIG_H
