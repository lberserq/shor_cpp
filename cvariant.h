#ifndef CVARIANT_H
#define CVARIANT_H
#include "config.h"
#include <common.h>
#include <vector>
class cVariant;

class VarFunc {
public:
    virtual void Exec(std::vector<cVariant> &vars) = 0;

    ~VarFunc() { }
};

inline str_t Tuple2S(const std::vector<cVariant> m_tupleVal);

class cVariant {
    enum mode_t {
        vtInt,
        vtFloat,
        vtComplex,
        vtString,
        vtFunc,
        vtTuple
    };
    mode_t m_mode;

    str_t m_strVal;
    int_t m_iVal;
    real_t m_dVal;
    complex_t m_cVal;
    VarFunc *m_func;
    std::vector<cVariant> m_tupleVal;
public:
    cVariant(const cVariant &other):
    m_mode(other.m_mode),
    m_strVal(other.m_strVal),
    m_iVal(other.m_iVal),
    m_dVal(other.m_dVal),
    m_cVal(other.m_cVal),
    m_func(other.m_func),
    m_tupleVal(other.m_tupleVal)
    {}

    cVariant(const int_t &val) : m_mode(vtInt), m_iVal(val) { }

    cVariant(const real_t &val) : m_mode(vtFloat), m_dVal(val) { }

    cVariant(const complex_t &val) : m_mode(vtComplex), m_cVal(val) { }

    cVariant(const str_t &val) : m_mode(vtString), m_strVal(val) { }

    cVariant(VarFunc *val) : m_mode(vtFunc), m_func(val) { }

    cVariant(const std::vector<cVariant> &val) : m_mode(vtTuple), m_tupleVal(val) { }



    cVariant& operator=(const cVariant &other) {
        cVariant(other).swap(*this);
        return *this;
    }

    cVariant & operator = (const int_t &val) {
        m_mode = vtInt;
        m_iVal = val;
        return *this;
    }


    cVariant & operator = (const real_t &val) {
        m_mode = vtFloat;
        m_dVal = val;
        return *this;
    }

    cVariant & operator = (const complex_t &val) {
        m_mode = vtComplex;
        m_cVal = val;
        return *this;
    }

    cVariant & operator = (const str_t &val) {
        m_mode = vtString;
        m_strVal = val;
        return *this;
    }

    cVariant & operator = (VarFunc *val) {
        m_mode = vtFunc;
        m_func = val;
        return *this;
    }


    cVariant & operator = (const std::vector<cVariant> &val) {
        m_mode = vtTuple;
        m_tupleVal = val;
        return *this;
    }


    operator int_t() const {
        switch (m_mode) {
        case vtInt:
            return m_iVal;
        default:
            return 0;
        }
    }


    operator str_t() const {
        switch (m_mode) {
        case vtString:
            return m_strVal;
        default:
            return toString();
        }
    }

    operator real_t() const {
        switch (m_mode) {
        case vtFloat:
            return m_dVal;
        default:
            return 0;
        }
    }

    operator complex_t() const {
        switch (m_mode) {
        case vtComplex:
            return m_cVal;
        default:
            return 0;
        }
    }


    operator VarFunc * () const {
        switch (m_mode) {
        case vtFloat:
            return m_func;
        default:
            return NULL;
        }
    }


    operator std::vector<cVariant>() const {
        switch (m_mode) {
        case vtTuple:
            return m_tupleVal;
        default:
            {
                std::vector<cVariant> res;
                res.push_back(*this);
                return res;
            }
        }
    }





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

    cVariant& swap(cVariant &other) {
        std::swap(m_mode, other.m_mode);
        std::swap(m_strVal, other.m_strVal);
        std::swap(m_iVal, other.m_iVal);
        std::swap(m_dVal, other.m_dVal);
        std::swap(m_cVal, other.m_cVal);
        std::swap(m_func, other.m_func);
        std::swap(m_tupleVal, other.m_tupleVal);
        return *this;



    }
};

inline str_t Tuple2S(const std::vector<cVariant> m_tupleVal) {
    str_t res = "[";
    for (size_t i = 0; i < m_tupleVal.size(); ++i) {
        res += m_tupleVal[i].toString() + ", ";
    }
    res += "]";
    return res;
}
#endif // CVARIANT_H

