#ifndef SINGLE_QREGISTER_H
#define SINGLE_QREGISTER_H
#include <vector>
#include <common/config.h>
#include <iface/quantum/iqregister.h>




class SingleQRegister : public IQRegister {
    std::vector<mcomplex> m_states;
    size_t m_width;
    size_t m_alloc_width;
    representation_t m_representation;
public:
    SingleQRegister(int width, state startState, const mcomplex &Wamplitude = 1.0,
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


#endif // SINGLE_QREGISTER_H
