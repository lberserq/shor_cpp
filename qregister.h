#ifndef IQRegister_H
#define IQRegister_H
#include <vector>
#include "config.h"

//class IQRegHelper {
//    void
//};


enum representation_t
{
    REG_REPRESENTATION,
    MATRIX_REPRESENTATION
};

//FUCK YOU SOLID
class IQRegister {
    friend class IQRegHelper;
public:
    virtual size_t getWidth() const = 0;
    virtual std::vector<mcomplex> & getStates() = 0;
    virtual void setStates(const std::vector<mcomplex> &v) = 0;
    virtual size_t getStatesSize() const  = 0;
//Remove candidates
    virtual void allocSharedMem(int width) = 0;
    virtual void collapseState(int id, long double amplProb) = 0;
//Remove candidates
    virtual void print() const = 0 ;
    virtual void printNorm() const = 0;

    virtual size_t getOffset() const = 0;
    virtual double getLocalNorm() const = 0;
    virtual std::vector<mcomplex> getAllReg() const = 0;
    virtual representation_t getRepresentation() const = 0;
    virtual ~IQRegister();
};

class StaticQRegister : public IQRegister {
    std::vector<mcomplex> m_states;
    size_t m_width;
    size_t m_alloc_width;
    representation_t m_representation;
public:
    StaticQRegister(int width, state startState, const mcomplex &Wamplitude = 1.0,
                    representation_t mode = REG_REPRESENTATION);

    size_t getWidth() const;
    std::vector<mcomplex> & getStates();
    void setStates(const std::vector<mcomplex> &v);
    size_t getStatesSize() const;
    void allocSharedMem(int width);
    void collapseState(int id, long double amplProb);
    void print() const;
    void printNorm() const;
    size_t getOffset() const;
    double getLocalNorm() const;
    std::vector<mcomplex> getAllReg() const;
    representation_t getRepresentation() const;
};

namespace ParallelSubSystemHelper
{
    struct mpicfg;
}
class SharedQSimpleQRegister : public IQRegister {
    representation_t m_representation;
    std::vector<mcomplex> m_localstates;
    size_t m_global_width;
    size_t m_global_alloc_width;

    size_t m_local_offset;
    size_t m_local_size;
    ParallelSubSystemHelper::mpicfg m_cfg;

public:
    SharedQSimpleQRegister(int width, state startState,
                           const mcomplex &amplitude = 1.0,
                           representation_t mode = REG_REPRESENTATION);
    size_t getWidth() const;
    std::vector<mcomplex> & getStates();
    void setStates(const std::vector<mcomplex> &v);
    size_t getStatesSize() const;
    void allocSharedMem(int width);
    void collapseState(int id, long double amplProb);
    void print() const;
    void printNorm() const;
    size_t getOffset() const;
    double getLocalNorm() const;
    std::vector<mcomplex> getAllReg() const;
    representation_t getRepresentation() const;

};



namespace QRegHelpers {
    void RegSwapLR(int width, IQRegister &in);
    void DeleteVar(IQRegister &reg, int size);
    void DeleteLocalVars(IQRegister &reg, int size);
    void SwapXY(IQRegister &reg, int width);
}
#endif // IQRegister_H
