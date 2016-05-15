#ifndef CVARIANT_H
#define CVARIANT_H
#include "config.h"
#include <common.h>
#include <vector>
class cVariant;

class VarFunc {
public:
  virtual void Exec(std::vector<cVariant> &vars) = 0;

  ~VarFunc() {}
};

inline str_t Tuple2S(const std::vector<cVariant> m_tupleVal);

class cVariant {

  enum mode_t { vtInt, vtFloat, vtComplex, vtString, vtFunc, vtTuple };
  mode_t m_mode;

  str_t m_strVal;
  int_t m_iVal;
  real_t m_dVal;
  complex_t m_cVal;
  VarFunc *m_func;
  typedef std::vector<cVariant> tuple_t;
  tuple_t m_tupleVal;

public:
  cVariant(const cVariant &other)
      : m_mode(other.m_mode), m_strVal(other.m_strVal), m_iVal(other.m_iVal),
        m_dVal(other.m_dVal), m_cVal(other.m_cVal), m_func(other.m_func),
        m_tupleVal(other.m_tupleVal) {}

  cVariant(const uint_type &val)
      : m_mode(vtInt), m_iVal(static_cast<int_t>(val)) {}

#ifndef INT_T_IS_INT
  cVariant(const int &val) : m_mode(vtInt), m_iVal(static_cast<int_t>(val)) {}
#endif

  cVariant(const int_t &val) : m_mode(vtInt), m_iVal(val) {}

  cVariant(const real_t &val) : m_mode(vtFloat), m_dVal(val) {}

  cVariant(const complex_t &val) : m_mode(vtComplex), m_cVal(val) {}

  cVariant(const str_t &val) : m_mode(vtString), m_strVal(val) {}

  cVariant(const char *val) : m_mode(vtString), m_strVal(val) {}

  cVariant(VarFunc *val) : m_mode(vtFunc), m_func(val) {}


  cVariant(const std::vector<cVariant> &val)
      : m_mode(vtTuple), m_tupleVal(val) {}

  template <typename T> T get() const;

  // cVarient
  friend bool operator<(const cVariant &left, const cVariant &right);
  friend bool operator<=(const cVariant &left, const cVariant &right);
  friend bool operator==(const cVariant &left, const cVariant &right);
  friend bool operator>=(const cVariant &left, const cVariant &right);
  friend bool operator>(const cVariant &left, const cVariant &right);
  friend bool operator!=(const cVariant &left, const cVariant &right);

#define CVARIANT_DECLARE_COMPARABLE(_type)                                     \
  friend bool operator<(const cVariant &left, const _type &right);             \
  friend bool operator<=(const cVariant &left, const _type &right);            \
  friend bool operator==(const cVariant &left, const _type &right);            \
  friend bool operator>=(const cVariant &left, const _type &right);            \
  friend bool operator>(const cVariant &left, const _type &right);             \
  friend bool operator!=(const cVariant &left, const _type &right);            \
                                                                               \
  friend bool operator<(const _type &left, const cVariant &right);             \
  friend bool operator<=(const _type &left, const cVariant &right);            \
  friend bool operator==(const _type &left, const cVariant &right);            \
  friend bool operator>=(const _type &left, const cVariant &right);            \
  friend bool operator>(const _type &left, const cVariant &right);             \
  friend bool operator!=(const _type &left, const cVariant &right);

  CVARIANT_DECLARE_COMPARABLE(int)
  CVARIANT_DECLARE_COMPARABLE(uint_type)
  CVARIANT_DECLARE_COMPARABLE(int_t)
  CVARIANT_DECLARE_COMPARABLE(float_t)
  CVARIANT_DECLARE_COMPARABLE(complex_t)
  CVARIANT_DECLARE_COMPARABLE(str_t)
  CVARIANT_DECLARE_COMPARABLE(std::vector<cVariant>)

  cVariant &operator=(const cVariant &other) {
    cVariant(other).swap(*this);
    return *this;
  }

#ifndef INT_T_IS_INT
  cVariant &operator=(const int &val) {
    m_mode = vtInt;
    m_iVal = val;
    return *this;
  }
#endif

  cVariant &operator=(const uint_type &val) {
    m_mode = vtInt;
    m_iVal = static_cast<int_t>(val);
    return *this;
  }

  cVariant &operator=(const int_t &val) {
    m_mode = vtInt;
    m_iVal = val;
    return *this;
  }

  cVariant &operator=(const real_t &val) {
    m_mode = vtFloat;
    m_dVal = val;
    return *this;
  }

  cVariant &operator=(const complex_t &val) {
    m_mode = vtComplex;
    m_cVal = val;
    return *this;
  }

  cVariant &operator=(const str_t &val) {
    m_mode = vtString;
    m_strVal = val;
    return *this;
  }

  cVariant &operator=(VarFunc *val) {
    m_mode = vtFunc;
    m_func = val;
    return *this;
  }

  cVariant &operator=(const std::vector<cVariant> &val) {
    m_mode = vtTuple;
    m_tupleVal = val;
    return *this;
  }

  operator bool() const;

  bool operator !() const
  {
      return !(this->operator bool());
  }

//  operator uint_type() const {
//    return static_cast<uint_type>(this->operator int_t());
//  }

//  operator int_t() const {
//    switch (m_mode) {
//    case vtInt:
//      return m_iVal;
//    default:
//      return 0;
//    }
//  }

  operator str_t() const {
    switch (m_mode) {
    case vtString:
      return m_strVal;
    default:
      return toString();
    }
  }

//  operator real_t() const {
//    switch (m_mode) {
//    case vtFloat:
//      return m_dVal;
//    default:
//      return 0;
//    }
//  }

//  operator complex_t() const {
//    switch (m_mode) {
//    case vtComplex:
//      return m_cVal;
//    default:
//      return 0;
//    }
//  }

//  operator VarFunc *() const {
//    switch (m_mode) {
//    case vtFloat:
//      return m_func;
//    default:
//      return NULL;
//    }
//  }

//  operator std::vector<cVariant>() const {
//    switch (m_mode) {
//    case vtTuple:
//      return m_tupleVal;
//    default: {
//      std::vector<cVariant> res;
//      res.push_back(*this);
//      return res;
//    }
//    }
//  }

  str_t toString() const {
    switch (m_mode) {
    case vtString:
      return m_strVal;
    case vtComplex:
    case vtFloat:
    case vtInt:
      return X_MAKESTR(m_iVal);
    case vtTuple:
      return Tuple2S(m_tupleVal);
    case vtFunc:
      return "function + 0x" + X_MAKESTR(m_func);
    default:
      return "Invalid variant";
    }
  }

  cVariant &swap(cVariant &other) {
    std::swap(m_mode, other.m_mode);
    std::swap(m_strVal, other.m_strVal);
    std::swap(m_iVal, other.m_iVal);
    std::swap(m_dVal, other.m_dVal);
    std::swap(m_cVal, other.m_cVal);
    std::swap(m_func, other.m_func);
    std::swap(m_tupleVal, other.m_tupleVal);
    return *this;
  }


  cVariant & operator++()
  {
      switch (m_mode)
      {
      case vtComplex:
          m_cVal+=1;
          break;
      case vtFloat:
          m_dVal++;
          break;
      case vtInt:
          m_iVal++;
          break;
      default:
          break;
      }
      return *this;
  }

  cVariant  operator++(int)
  {
      cVariant prev = *this;
      switch (m_mode)
      {
      case vtComplex:
          m_cVal+=1;
          break;
      case vtFloat:
          m_dVal++;
          break;
      case vtInt:
          m_iVal++;
          break;
      default:
          break;
      }
      return prev;
  }


  cVariant  operator +(const cVariant &other) const;

  cVariant  operator -(const cVariant &other) const;

  cVariant  operator *(const cVariant &other) const;

  cVariant  operator / (const cVariant &other) const;

  cVariant  operator % (const cVariant &other) const;


  cVariant & operator--()
  {
      switch (m_mode)
      {
      case vtComplex:
          m_cVal-=1;
          break;
      case vtFloat:
          m_dVal--;
          break;
      case vtInt:
          m_iVal--;
          break;
      default:
          break;
      }
      return *this;
  }

  cVariant  operator--(int)
  {
      cVariant prev = *this;
      switch (m_mode)
      {
      case vtComplex:
          m_cVal-=1;
          break;
      case vtFloat:
          m_dVal--;
          break;
      case vtInt:
          m_iVal--;
          break;
      default:
          break;
      }
      return prev;
  }


  cVariant operator [](size_t idx) {
      if (idx < m_tupleVal.size()) {
        return m_tupleVal[idx];
      }
      return cVariant(NAN);
  }




#define CVARIANT_DECLARE_OP_TYPE(_op, _type)\


#define CVARIANT_OP_ASSIGN_DECLARE(_op)\
    cVariant & operator _op##=(const cVariant &other);


  CVARIANT_OP_ASSIGN_DECLARE(+)
  CVARIANT_OP_ASSIGN_DECLARE(-)
  CVARIANT_OP_ASSIGN_DECLARE(*)
  CVARIANT_OP_ASSIGN_DECLARE(/)
  CVARIANT_OP_ASSIGN_DECLARE(%)
  CVARIANT_OP_ASSIGN_DECLARE(<<)
  CVARIANT_OP_ASSIGN_DECLARE(>>)


#define CVARIANT_OP_ASSIGN_TYPE_DECLARE(_op, _type)\
    cVariant & operator _op##=(const _type &other);

#define CVARIANT_DECLARE_OP_ASSIGNS_TYPE(_type)\
    CVARIANT_OP_ASSIGN_TYPE_DECLARE(+, _type)\
    CVARIANT_OP_ASSIGN_TYPE_DECLARE(-, _type)\
    CVARIANT_OP_ASSIGN_TYPE_DECLARE(/, _type)\
    CVARIANT_OP_ASSIGN_TYPE_DECLARE(*, _type)\
    CVARIANT_OP_ASSIGN_TYPE_DECLARE(%, _type)\




  CVARIANT_DECLARE_OP_ASSIGNS_TYPE(int)
  CVARIANT_DECLARE_OP_ASSIGNS_TYPE(uint_type)
  CVARIANT_DECLARE_OP_ASSIGNS_TYPE(int_t)
  CVARIANT_DECLARE_OP_ASSIGNS_TYPE(float_t)
  CVARIANT_DECLARE_OP_ASSIGNS_TYPE(complex_t)
  CVARIANT_DECLARE_OP_ASSIGNS_TYPE(str_t)
  CVARIANT_DECLARE_OP_ASSIGNS_TYPE(std::vector<cVariant>)

#define CVARIANT_DECLARE_OPS_TYPE(_type)\
    CVARIANT_DECLARE_OP_TYPE(+, _type)


//  CVARIANT_DECLARE_OPS_TYPE(int)
//  CVARIANT_DECLARE_OPS_TYPE(uint_type)
//  CVARIANT_DECLARE_OPS_TYPE(int_t)
//  CVARIANT_DECLARE_OPS_TYPE(float_t)
//  CVARIANT_DECLARE_OPS_TYPE(complex_t)
  CVARIANT_DECLARE_OPS_TYPE(str_t)
//  CVARIANT_DECLARE_OPS_TYPE(std::vector<cVariant>)








};

inline str_t Tuple2S(const std::vector<cVariant> m_tupleVal) {
  str_t res = "[";
  for (size_t i = 0; i < m_tupleVal.size(); ++i) {
    res += m_tupleVal[i].toString() + ", ";
  }
  res += "]";
  return res;
}

template <> inline int_t cVariant::get<int_t>() const {
  switch (m_mode) {
  case vtInt:
    return m_iVal;
  case vtFloat:
    return m_dVal;
  case vtComplex:
    return std::abs(m_cVal);
  case vtTuple:
    return (m_tupleVal.size()) ? m_tupleVal[0].get<int_t>() : 0;
  default:
    return 0;
  }
  return 0;
}

template <> inline float_t cVariant::get<float_t>() const {
  switch (m_mode) {
  case vtInt:
    return m_iVal;
  case vtFloat:
    return m_dVal;
  case vtComplex:
    return std::abs(m_cVal);
  case vtTuple:
    return (m_tupleVal.size()) ? m_tupleVal[0].get<float_t>() : 0;
  default:
    return 0;
  }
  return 0;
}

template <> inline complex_t cVariant::get<complex_t>() const {
  switch (m_mode) {
  case vtInt:
    return m_iVal;
  case vtFloat:
    return m_dVal;
  case vtComplex:
    return m_cVal;
  case vtTuple:
    return (m_tupleVal.size()) ? m_tupleVal[0].get<complex_t>() : 0;
  default:
    return 0;
  }
  return 0;
}

template <> inline str_t cVariant::get<str_t>() const { return toString(); }

template <> inline VarFunc *cVariant::get<VarFunc *>() const {
  return (m_mode == vtFunc) ? m_func : NULL;
}

template <> inline std::vector<cVariant> cVariant::get<std::vector<cVariant> >() const {
  tuple_t res;
  res.push_back(*this);
  return (m_mode == vtTuple) ? m_tupleVal : res;
}

inline cVariant::operator bool() const { return get<int_t>() != 0; }

namespace {
int x_tuple_cmp(const std::vector<cVariant> &left,
                const std::vector<cVariant> &right) {
  if (left.size() < right.size()) {
    return -1;
  } else if (left.size() > right.size()) {
    return 1;
  }

  for (size_t i = 0; i < left.size(); ++i) {
    if (left[i] != right[i]) {
      return (left[i] < right[i]) ? -1 : 1;
    }
  }
  return 0;
}
}

inline bool operator<(const cVariant &left, const cVariant &right) {

  switch (left.m_mode) {
  case cVariant::vtInt:
    return left.m_iVal < right.get<int_t>();
  case cVariant::vtFloat:
    return left.m_dVal < right.get<float_t>();
  case cVariant::vtComplex:
    return left.m_cVal < right.get<complex_t>();
  case cVariant::vtString:
    return left.m_strVal < right.get<str_t>();

  case cVariant::vtTuple:
  {
    std::vector<cVariant> leftT;
    leftT.push_back(left);
    return x_tuple_cmp(leftT, right.get<std::vector<cVariant> >()) < 0;
  }
  default:
    return false;
    break;
  }

  return false;
}

inline bool operator==(const cVariant &left, const cVariant &right) {
  switch (left.m_mode) {
  case cVariant::vtInt:
    return left.m_iVal == right.get<int_t>();
  case cVariant::vtFloat:
    return left.m_dVal == right.get<float_t>();
  case cVariant::vtComplex:
    return left.m_cVal == right.get<complex_t>();
    ;
  case cVariant::vtString:
    return left.m_strVal == right.get<str_t>();
    ;
  case cVariant::vtTuple:
      {
        std::vector<cVariant> leftT;
        leftT.push_back(left);
        return x_tuple_cmp(leftT, right.get<std::vector<cVariant> >())  == 0;
      }
  default:
    return false;
    break;
  }

  return false;
}

inline bool operator>(const cVariant &left, const cVariant &right) {
  return right < left;
}

inline bool operator<=(const cVariant &left, const cVariant &right) {
  return !(left > right);
}

inline bool operator>=(const cVariant &left, const cVariant &right) {
  return !(left < right);
}

inline bool operator!=(const cVariant &left, const cVariant &right) {
  return !(left == right);
}

#define CVARIANT_DECLARE_COMPARABLE_IMPL(_type)                                \
inline bool operator<(const cVariant &left, const _type &right) {                   \
    return left < cVariant(right);                                             \
  }                                                                            \
                                                                               \
inline  bool operator<=(const cVariant &left, const _type &right) {                  \
    return left < cVariant(right);                                             \
  }                                                                            \
                                                                               \
inline  bool operator==(const cVariant &left, const _type &right) {                  \
    return left == cVariant(right);                                            \
  }                                                                            \
                                                                               \
inline  bool operator>=(const cVariant &left, const _type &right) {                  \
    return left >= cVariant(right);                                            \
  }                                                                            \
                                                                               \
inline  bool operator>(const cVariant &left, const _type &right) {                   \
    return left > cVariant(right);                                             \
  }                                                                            \
                                                                               \
inline  bool operator!=(const cVariant &left, const _type &right) {                  \
    return (left != cVariant(right));                                          \
  }                                                                            \
                                                                               \
inline  bool operator<(const _type &left, const cVariant &right) {                   \
    return cVariant(left) < right;                                             \
  }                                                                            \
                                                                               \
inline  bool operator<=(const _type &left, const cVariant &right) {                  \
    return cVariant(left) <= right;                                            \
  }                                                                            \
                                                                               \
inline  bool operator==(const _type &left, const cVariant &right) {                  \
    return (cVariant(left) == right);                                          \
  }                                                                            \
                                                                               \
inline  bool operator>=(const _type &left, const cVariant &right) {                  \
    return (cVariant(left) >= right);                                          \
  }                                                                            \
                                                                               \
inline  bool operator>(const _type &left, const cVariant &right) {                   \
    return cVariant(left) > right;                                             \
  }                                                                            \
                                                                               \
inline  bool operator!=(const _type &left, const cVariant &right) {                  \
    return cVariant(left) != right;                                            \
  }

CVARIANT_DECLARE_COMPARABLE_IMPL(int)
CVARIANT_DECLARE_COMPARABLE_IMPL(uint_type)
CVARIANT_DECLARE_COMPARABLE_IMPL(int_t)
CVARIANT_DECLARE_COMPARABLE_IMPL(float_t)
CVARIANT_DECLARE_COMPARABLE_IMPL(complex_t)
CVARIANT_DECLARE_COMPARABLE_IMPL(str_t)
CVARIANT_DECLARE_COMPARABLE_IMPL(std::vector<cVariant>)

inline cVariant cVariant::operator+(const cVariant &other) const {
  switch (m_mode) {
  case vtString:
    return cVariant(m_strVal + other.toString());
  case vtComplex:
    return cVariant(m_cVal + other.get<complex_t>());
  case vtFloat:
    return cVariant(m_dVal + other.get<float_t>());
  case vtInt:
    return cVariant(m_iVal + other.get<int_t>());
  case vtTuple: {
    std::vector<cVariant> res = m_tupleVal;
    for (size_t i = 0; i < other.m_tupleVal.size(); i++) {
      res.push_back(other.m_tupleVal[i]);
    }
    return cVariant(res);
  }
  default:
    return *this;
  }
  return *this;
}

inline cVariant  cVariant::operator -(const cVariant &other) const
{
    switch (m_mode) {
    case vtComplex:
      return cVariant(m_cVal - other.get<complex_t>());
    case vtFloat:
        return cVariant(m_dVal - other.get<float_t>());
    case vtInt:
        return cVariant(m_iVal - other.get<int_t>());
    default:
        return *this;
    }
    return *this;
}

inline cVariant  cVariant::operator *(const cVariant &other) const
{
    switch (m_mode) {
    case vtComplex:
      return cVariant(m_cVal * other.get<complex_t>());
    case vtFloat:
        return cVariant(m_dVal * other.get<float_t>());
    case vtInt:
        return cVariant(m_iVal * other.get<int_t>());
    default:
        return *this;
    }
    return *this;
}

inline cVariant  cVariant::operator / (const cVariant &other) const
{
    switch (m_mode) {
    case vtComplex:
      return cVariant(m_cVal / other.get<complex_t>());
    case vtFloat:
        return cVariant(m_dVal / other.get<float_t>());
    case vtInt:
        return cVariant(m_iVal / other.get<int_t>());
    default:
        return *this;
    }
    return *this;
}

inline cVariant  cVariant::operator % (const cVariant &other) const
{
   return cVariant(get<int_t>() % other.get<int_t>());
}



#define CVARIANT_OP_ASSIGN_IMPL1(_op)\
    inline cVariant & cVariant::operator _op##=(const cVariant &other)\
    {\
      *this = *this  _op other;\
      return *this;\
    }

CVARIANT_OP_ASSIGN_IMPL1(+)
CVARIANT_OP_ASSIGN_IMPL1(-)
CVARIANT_OP_ASSIGN_IMPL1(*)
CVARIANT_OP_ASSIGN_IMPL1(/)

#define CVARIANT_OP_ASSIGN_TYPE_IMPL(_op, _type)                            \
  inline cVariant & cVariant::operator _op##=(const _type &other)\
  {\
    return cVariant::operator _op##=(cVariant(other));\
  }\

#define CVARIANT_IMPL_OP_ASSIGNS_TYPE(_type)                                   \
  CVARIANT_OP_ASSIGN_TYPE_IMPL(+, _type)                                       \
  CVARIANT_OP_ASSIGN_TYPE_IMPL(-, _type)                                       \
  CVARIANT_OP_ASSIGN_TYPE_IMPL(/, _type)                                       \
  CVARIANT_OP_ASSIGN_TYPE_IMPL(*, _type)                                       \
  CVARIANT_OP_ASSIGN_TYPE_IMPL(%, _type)

CVARIANT_IMPL_OP_ASSIGNS_TYPE(int)
CVARIANT_IMPL_OP_ASSIGNS_TYPE(uint_type)
CVARIANT_IMPL_OP_ASSIGNS_TYPE(int_t)
CVARIANT_IMPL_OP_ASSIGNS_TYPE(float_t)
CVARIANT_IMPL_OP_ASSIGNS_TYPE(complex_t)
CVARIANT_IMPL_OP_ASSIGNS_TYPE(str_t)
CVARIANT_IMPL_OP_ASSIGNS_TYPE(std::vector<cVariant>)

#define CVARIANT_IMPL_OP_TYPE(_op, _type)                                      \
  inline cVariant operator _op(const cVariant &left, const _type &right) {            \
    return cVariant(left _op cVariant(right));                                 \
  }                                                                            \
  inline cVariant operator _op(const _type &left, const cVariant &right) {            \
    return cVariant(cVariant(left) _op right);                                 \
  }

#define CVARIANT_IMPL_OPS_TYPE(_type)                                          \
  CVARIANT_IMPL_OP_TYPE(+, _type)                                              \
  CVARIANT_IMPL_OP_TYPE(-, _type)                                              \
  CVARIANT_IMPL_OP_TYPE(*, _type)                                              \
  CVARIANT_IMPL_OP_TYPE(/, _type)                                              \
  CVARIANT_IMPL_OP_TYPE(%, _type)

CVARIANT_IMPL_OPS_TYPE(int)
//CVARIANT_IMPL_OPS_TYPE(uint_type)
//CVARIANT_IMPL_OPS_TYPE(int_t)
//CVARIANT_IMPL_OPS_TYPE(float_t)
//CVARIANT_IMPL_OPS_TYPE(complex_t)
CVARIANT_IMPL_OPS_TYPE(str_t)
CVARIANT_IMPL_OPS_TYPE(char*)
//CVARIANT_IMPL_OPS_TYPE(std::vector<cVariant>)

inline cVariant & cVariant::operator %=(const cVariant &other)
{
    if (other.m_iVal != 0)
    {
        m_iVal %= other.m_iVal;
    }
    return *this;
}

inline cVariant &cVariant::operator<<=(const cVariant &other) {
  if (other.m_iVal >= 0) {
    m_iVal <<= other.m_iVal;
  }
  return *this;
}

inline cVariant &cVariant::operator>>=(const cVariant &other) {
  if (other.m_iVal >= 0) {
    m_iVal >>= other.m_iVal;
  }
  return *this;
}

#endif // CVARIANT_H
