#ifndef CONFIG_H
#define CONFIG_H
#include <stdint.h>
#include <stdexcept>
#include <mpi.h>
enum
    {
    REG_NUM  = 4
    };
const double g_eps = 1e-8;
typedef unsigned long long state_t;
typedef state_t uint_type;
#include <complex>
typedef double real_t;
typedef std::complex<real_t> mcomplex;
typedef uint8_t byte_t;

#ifdef INT_T_IS_INT
typedef int int_t
#else
typedef long long int_t;
#endif
typedef std::string str_t;
typedef mcomplex complex_t;


#include <mpi.h>

#define MPI_REAL_T MPI_DOUBLE
#define MPI_MCOMPLEX MPI_DOUBLE_COMPLEX
#define MPI_COMPLEX_T MPI_DOUBLE_COMPLEX
#define MPI_STATE_T MPI_UNSIGNED_LONG_LONG

#include <iterator>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#define EKA_DECLARE_IID(x)\
    enum {\
        int __eka__id = x;\
    };\

#ifdef _MSC_VER
typedef  unsigned int result_t;
#else
typedef  unsigned result_t;
#endif

#define MPI_RESULT_T MPI_UNSIGNED

#include <stdexcept>


inline int get_bit(state_t x, int id) {
    return (x >> id) & 1;
}

inline int set_bit(state_t x, unsigned char id, int val) {
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
//typedef StaticQRegister UserDefQRegister;



#define STR(x) #x
#define STRINGIFY(x) STR(x)
#define CONCATENATE(X,Y) X ( Y )
#define X_ASSERT(val) {if (!(val)) throw std::logic_error(std::string(__FILE__) + STRINGIFY(__LINE__));}
#include <vector>

typedef intptr_t handle;
typedef std::vector<byte_t> byteArray;


namespace ParallelSubSystemHelper {
    struct mpicfg {
        int rank;
        int size;
        mpicfg(int rank = 0, int size = 1) : rank(rank), size(size){}
    };

    mpicfg getConfig();
    bool isInited();
    namespace sync
    {
        void barrier();
    }

    namespace RemoteIO
    {
        void gather();
        void reduce();

    }

    namespace FileIO
    {
        class ScopedFileHandle
        {
            handle m_handle;
        public:
            ScopedFileHandle(handle hin) : m_handle(hin){}
            operator handle() {return m_handle;}
            ~ScopedFileHandle();
        };

        ScopedFileHandle open(const char *fileName);

        result_t read(ScopedFileHandle &, byteArray &);
        result_t write(ScopedFileHandle &, const byteArray &);

    }




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


enum error_codes
{
    X_SOK = 0,
    X_FAIL = 1,
    X_CONTRACT_VIOLATON = 2,
};

#define X_UNUSED(param) {(void)(param);}
#define X_SUCCEEDED(expr)  ((expr) == X_SOK)
#define X_FAILED(expr) (!X_SUCCEEDED(expr))


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

#define q_log_always_impl(_x, _rank) \
{\
    if (ParallelSubSystemHelper::isInited()){\
        dumpVar((_x), _rank);}\
    else{\
        std::cerr << (_x) << std::endl;}\
}

#define q_log_always(x) q_log_always_impl(x, 0)

#define reduce_helper
#define ENABLE_LOG
#ifdef ENABLE_LOG
#define q_log(x) q_log_always(x)
#else
#define q_log(x)
#endif


#define MPI_SAFE_CALL(func)\
{\
    int _ret = (func);\
    if (_ret != MPI_SUCCESS)\
    {\
        char ptr[256]; int len; \
        MPI_Error_string(_ret, ptr, &len);\
        q_log_always_impl(ptr, ParallelSubSystemHelper::getConfig().rank);\
        MPI_Abort(MPI_COMM_WORLD, -11);\
    }\
}\

inline void MPI_PRINT_ERROR(int status)
{
    char ptr[256];
    int len;
    MPI_Error_string(status, ptr, &len);
    q_log_always_impl(ptr, ParallelSubSystemHelper::getConfig().rank);
}

enum
{
        XML_LOAD_FAILED = 100,
};

#define make_error(err)

#endif // CONFIG_H
