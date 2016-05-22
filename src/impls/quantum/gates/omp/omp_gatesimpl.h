#ifndef OMP_GATESIMPL_H
#define OMP_GATESIMPL_H
#include <iface/quantum/igates.h>

class OMPGatesImpl : public IGates
{
public:
    void ApplyQbitMatrix(const QMatrix &m, IQRegister &reg, int id0);
    void ApplyDiQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1);
    void ApplyTriQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1, int id2);
};
#endif // OMP_GATESIMPL_H
