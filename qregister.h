#ifndef IQRegister_H
#define IQRegister_H
#include <vector>
#include "config.h"
#include <iqregister.h>




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

#endif // IQRegister_H
