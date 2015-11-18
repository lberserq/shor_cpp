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
typedef std::complex<long double> mcomplex;
#include <stdlib.h>
#include <stdio.h>

#define EKA_DECLARE_IID(x)\
    int __eka__id = x;\

#ifdef _MSC_VER
typedef  unsigned int result_t;
#else
typedef  unsigned result_t;
#endif

#include <stdexcept>

#endif // CONFIG_H
