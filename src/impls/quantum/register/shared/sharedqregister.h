#ifndef SHAREDQREGISTER_H
#define SHAREDQREGISTER_H
#include <vector>
#include <common/config.h>
#include <iface/quantum/iqregister.h>
namespace ParallelSubSystemHelper
{
    struct mpicfg;
}
class SharedSimpleQRegister : public IQRegister {
    representation_t m_representation;
    std::vector<mcomplex> m_localstates;
    size_t m_global_width;
    size_t m_global_alloc_width;

    size_t m_local_offset;
    size_t m_local_size;
    ParallelSubSystemHelper::mpicfg m_cfg;

public:
    SharedSimpleQRegister(int width, state startState,
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

#endif // SHAREDQREGISTER_H
