#ifndef QMATH_H
#define QMATH_H
#include <cmath>
#include "config.h"

#define X_DECLARE_TEMPLATE_MATH_1(__name)\
    template<class T>  T __name(const T&x) {\
        return std::__name(x);\
    }

#define X_DECLARE_TEMPLATE_MATH_2(__name)\
    template<class T>  T __name(const T &y, const T&x) {\
        return std::__name(y, x);\
    }

namespace QMath
{
#undef abs
#undef div
#undef acos
#undef asin
#undef atan
#undef atan2
#undef ceil
#undef cos
#undef cosh
#undef exp
#undef fabs
#undef floor
#undef fmod
#undef frexp
#undef ldexp
#undef log
#undef log10
#undef modf
#undef pow
#undef sin
#undef sinh
#undef sqrt
#undef tan
#undef tanh

  X_DECLARE_TEMPLATE_MATH_1(abs)
  X_DECLARE_TEMPLATE_MATH_1(acos)
  X_DECLARE_TEMPLATE_MATH_1(asin)
  X_DECLARE_TEMPLATE_MATH_1(atan)
  X_DECLARE_TEMPLATE_MATH_2(atan2)
  X_DECLARE_TEMPLATE_MATH_1(ceil)
  X_DECLARE_TEMPLATE_MATH_1(cos)
  X_DECLARE_TEMPLATE_MATH_1(cosh)
  X_DECLARE_TEMPLATE_MATH_1(exp)
  X_DECLARE_TEMPLATE_MATH_1(fabs)
  X_DECLARE_TEMPLATE_MATH_1(floor)
  X_DECLARE_TEMPLATE_MATH_1(fmod)
  X_DECLARE_TEMPLATE_MATH_1(ldexp)
  X_DECLARE_TEMPLATE_MATH_2(log)
  X_DECLARE_TEMPLATE_MATH_1(log10)
  X_DECLARE_TEMPLATE_MATH_1(modf)
  X_DECLARE_TEMPLATE_MATH_1(sin)
  X_DECLARE_TEMPLATE_MATH_1(sinh)
  X_DECLARE_TEMPLATE_MATH_1(sqrt)
  X_DECLARE_TEMPLATE_MATH_1(tan)
  X_DECLARE_TEMPLATE_MATH_1(tanh)











}

#endif // QMATH_H
