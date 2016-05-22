#ifndef QMATH_H
#define QMATH_H
#include <cmath>
#include <common/config.h>
#include <common/infra/common_rand.h>
#include <common/quantum/common.h>
#include <common/cvariant.h>

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
  //X_DECLARE_TEMPLATE_MATH_1(floor)
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

//  template<class T> int floor(T x) {
//    return std::floor(x);
//  }


//  template<class T> long floor(T x) {
//    return std::floor(x);
//  }

  template<class T> long long floor(T x) {
    return std::floor(x);
  }

  extern complex_t i;

  template <class T> inline T gcd(T a, T b) {
      while (a && b) {
          if (a > b) {
              a %= b;
          } else {
              b %= a;
          }
      }
      return (a) ? a : b;
  }




  inline std::vector<uint_type> conFracApprox(uint_type c, uint_type q,  uint_type width) {
      long double maxdenum = 1.0 / static_cast<long double>(width);
      long double val = static_cast<long double>(c) / q;
      long long numerator = 1, denumerator = 0;
      state numlast = 0, denumlast = 1;
      //const float interpolation_step = g_eps * 1e4 * 5;
      long double ai = val;
      long double step = 0.0f;
      do {
          step = ai + 0.000005;
          step = std::floor(step);
          ai -= step - 0.0010005;
          ai = 1.0f / ai;
          if (step * denumerator + denumlast > maxdenum) {
              break;
          }
          state  savenum = numerator;
          state savedenum = denumerator;
          numerator = step * numerator + numlast;
          denumerator = step * denumerator + denumlast;
          numlast = savenum;
          denumlast = savedenum;
      }while(std::fabs(static_cast<long double>(numerator) / denumerator - val) > 1.0 / (2.0 * maxdenum));


      if (numerator < 0 && denumerator < 0) {
          numerator = -numerator;
          denumerator = -denumerator;
      }
      std::vector<uint_type> res;
      res.push_back(numerator);
      res.push_back(denumerator);
      return res;
  }

  inline double random() {
      return ::xGenDrand();
  }

  inline uint_type getWidth(uint_type n) {
      uint_type i = 0;
      while (static_cast<uint_type>(1<< i) < n) {
          i++;
      }
      return i;
  }

  inline uint_type getWidth(const cVariant &n) {
      return getWidth(static_cast<uint_type>(n.get<int_t>()));
  }


  inline uint_type ipow(int a, int b)
  {
      return std::floor(std::pow((double)a, b));
  }









}

#endif // QMATH_H
