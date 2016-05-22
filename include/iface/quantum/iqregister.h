#ifndef IQREGISTER_H
#define IQREGISTER_H
#include <vector>
#include <common/config.h>

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
    virtual ~IQRegister(){}
};


#endif // IQREGISTER_H

