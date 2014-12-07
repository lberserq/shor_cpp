#ifndef COMMON_H
#define COMMON_H
#include "qmatrix.h"
#include "qregister.h"
/*!
 *\brief this file contains basic gates. Complicated gates in the specified files\n
 */

/*!
 *\fn applies Unitary complex matrix m(2x2) to register reg on qubit specified in id0\n
*/
void ApplyQbitMatrix(const QMatrix &m, Qregister &reg, int id0);
/*!
 *\fn applies Unitary complex matrix m(4x4) to register reg on 2 qubits specified in id0 id1\n
*/
void ApplyDiQbitMatrix(const QMatrix &m, Qregister &reg, int id0, int id1);

/*!
 *\fn applies Unitary complex matrix m(8x9) to register reg on 3 qubits specified in id0 id1 id2\n
*/
void ApplyTriQbitMatrix(const QMatrix &m, Qregister &reg, int id0, int id1, int id2);

/*!
 *\fn applies Toffolli gate |x,y,z>->|x,y,z xor xy>\n
 *|1 0 0 0 0 0 0 0|\n
 *|0 1 0 0 0 0 0 0|\n
 *|0 0 1 0 0 0 0 0|\n
 *|0 0 0 1 0 0 0 0|\n
 *|0 0 0 0 1 0 0 0|\n
 *|0 0 0 0 0 1 0 0|\n
 *|0 0 0 0 0 0 0 1|\n
 *|0 0 0 0 0 0 1 0|\n
*/
void ApplyToffoli(Qregister &reg, int id0, int id1, int id2);

/*!
 *\fn faster version of ApplyToffoli\n
*/
void ApplyFToffoli(Qregister &reg, int id0, int id1, int id2);


/*!
 *\fn applies CNOT gate to reg |x,y> ->|x, x xor y> with toffolli and not
 *creates the computational basis\n
 *|1 0 0 0|\n
 *|0 1 0 0|\n
 *|0 0 0 1|\n
 *|0 0 1 0|\n
*/
void ApplyCnot(Qregister &reg, int id0, int id1);

/*!
 *\fn faster version of ApplyCnot
*/
void ApplyFcnot(Qregister &reg, int id0, int id1);
/*!
 *\fn applies Controlled Rotation gate to reg\n
 *|1 0 0 0|\n
 *|0 1 0 0|\n
 *|0 0 1 0|\n
 *|0 0 0 exp(i * alpha)|\n
*/
void ApplyCRot(Qregister &reg, int id1, int id2, double alpha);

/*!
 *\fn applies Hadamard gate to reg\n
 *|1 -1|\n
 *|1  1|  * sqrt(1/2.0)\n
*/
void ApplyHadamard(Qregister &reg, int id1);

/*!
 *\fn applies Not gate to reg\n
 *|0 1|\n
 *|1 0|\n
*/
void ApplyNot(Qregister &reg, int id);

/*!
 *\fn applies Swap gate to reg\n
 *|1 0 0 0|\n
 *|0 0 1 0|\n
 *|0 1 0 0|\n
 *|0 0 0 1|\n
*/

void ApplySWAP(Qregister &reg, int id0, int id1);


/*!
 *\fn applies Controlled Swap gate to reg\n
 *|1 0 0 0|\n
 *|0 0 1 0|\n
 *|0 1 0 0|\n
 *|0 0 0 1|\n
*/

void ApplyCSWAP(Qregister &reg, int id0, int id1, int id2);

#endif // COMMON_H
