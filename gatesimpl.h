#ifndef GATESIMPL_H
#define GATESIMPL_H
#include "igates.h"

class OpenMPGatesImpl : public IGates
{
public:
    void ApplyQbitMatrix(const QMatrix &m, IQRegister &reg, int id0);
    void ApplyDiQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1);
    void ApplyTriQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1, int id2);
};


class NUMAGatesImpl : public IGates
{
public:
    void ApplyQbitMatrix(const QMatrix &m, IQRegister &reg, int id0);
    void ApplyDiQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1);
    void ApplyTriQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1, int id2);
};
#endif // GATESIMPL_H
