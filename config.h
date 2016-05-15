#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>
#include <stdexcept>
enum
    {
    REG_NUM  = 4
    };
const double g_eps = 1e-8;
typedef unsigned long long state;
typedef state uint_type;
#include <complex>
#include <iterator>
typedef  double real_t;
typedef std::complex<real_t> mcomplex;
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


inline int get_bit(state x, int id) {
    return (x >> id) & 1;
}

inline int set_bit(state x, unsigned char id, int val) {
    switch (val) {
    case 0:
        x &= ~(1 << id);
        break;
    case 1:
        x |= (1 << id);
        break;
    default:
        break;
    }
    return x;
}



#if __cplusplus >=  201103L
#include <memory>
#define service_ptr_t std::shared_ptr
#else
#include <memory>
#define service_ptr_t std::auto_ptr
#endif

struct ICommonWorld;
extern service_ptr_t<ICommonWorld> gWorld;

extern void userInit();
class StaticQRegister;
class SharedQSimpleQRegister;
//typedef StaticQRegister UserDefQRegister;



#define STR(x) #x
#define STRINGIFY(x) STR(x)
#define CONCATENATE(X,Y) X ( Y )
#define X_ASSERT(val) {throw std::logic_error(std::string(__FILE__) + STRINGIFY(__LINE__))

namespace ParallelSubSystemHelper {
    struct mpicfg {
        int rank;
        int size;
        mpicfg(int rank = 0, int size = 1) : rank(rank), size(size){}
    };

    mpicfg getConfig();
    bool isInited();
    void barrier();
    //template <class T> void AllReduceHelper();

    /*    void AllreduceHelper(long double &val, MPI_Op op) {
        long double gval = 0;
        if (getConfig().size) {
            MPI_Allreduce(&gval, &http://vk.com/public85340642val, 1, MPI_LONG_DOUBLE, op, MPI_COMM_WORLD);
        }
        val = gval;
    }*/

    namespace thread
    {
        template<class T> void parallelAddAssign(T *firstBegin, T *second, size_t size);
        template<class T> void parallelSubAssign(T *firstBegin, T *second, size_t size);
        template<class T> void parallelMulAssign(T *firstBegin, T *second, size_t size);
        template<class T> void parallelDivAssign(T *firstBegin, T *second, size_t size);
    }
}





#define dumpVar(var, Rank) \
{\
    MPI_Barrier(MPI_COMM_WORLD);\
    if (ParallelSubSystemHelper::getConfig().rank == Rank) {\
    std::cerr << "DUMPING VAR == " << var << std::flush << std::endl;\
    }\
    MPI_Barrier(MPI_COMM_WORLD);}\

#define mpi_dumparray(array, len, Rank) \
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


#define dumparray(array, len, Rank) \
{\
    if (ParallelSubSystemHelper::isInited()){\
        mpi_dumparray(array, len, Rank);\
    } else {\
    std::cerr << "Dumping Array "  << std::endl; \
    for (size_t i = 0; i < len; i++) \
    if (i != len - 1) std::cerr << array[i] << " "; \
    else std::cerr << array[i] << "\n"; \
    std::cerr << std::flush; \
    }\
}


#define dumpvec(vec, Rank) \
{\
    dumparray(vec.begin(), vec.size(), Rank);\
}



#define reduce_helper
//#define ENABLE_LOG
#ifdef ENABLE_LOG
#define q_log(x) \
{\
    if (ParallelSubSystemHelper::isInited()){\
        dumpVar((x), 0);}\
    else{\
        std::cerr << (x) << std::endl;}\
}
#else
#define q_log(x)
#endif
enum
{
        XML_LOAD_FAILED = 100,
};

#define make_error(err)
#ifdef INT_T_IS_INT
typedef int int_t
#else
typedef long long int_t;
#endif
typedef std::string str_t;
typedef std::complex<real_t> complex_t;

#endif // CONFIG_H
