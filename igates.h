#ifndef IGATES_H
#define IGATES_H
#include <iqregister.h>
#include <qmatrix.h>
struct IGates {
    /*!
     *\fn applies Unitary complex matrix m(2x2) to register reg on qbit specified in id0\n
    */
    virtual void ApplyQbitMatrix(const QMatrix &m, IQRegister &reg, int id0) = 0;

    /*!
    *\fn applies Unitary complex matrix m(4x4) to register reg on 2 qubits specified in id0 id1\n
    */
    virtual void ApplyDiQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1) = 0;

    /*!
    *\fn applies Unitary complex matrix m(8x8) to register reg on 3 qubits specified in id0 id1 id2\n
    */
    virtual void ApplyTriQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1, int id2) = 0;
    virtual ~IGates(){}
};
#endif // IGATES_H

