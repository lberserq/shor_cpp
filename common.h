#ifndef COMMON_H
#define COMMON_H
#include "qmatrix.h"
#include "qregister.h"



void log_init_reg(int m_w);
void log_print();
void log_reinit_reg(int new_mw);
int set_bit(state x, unsigned char id, int val);
int getbit(state x, int id);
extern double errorLevel;


/*!
 *\brief this file contains basic gates. Complicated gates in the specified files\n
 */

/*!
 *\fn applies Unitary complex matrix m(2x2) to register reg on qubit specified in id0\n
*/
void ApplyQbitMatrix(const QMatrix &m, IQRegister &reg, int id0);
/*!
 *\fn applies Unitary complex matrix m(4x4) to register reg on 2 qubits specified in id0 id1\n
*/
void ApplyDiQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1);

/*!
 *\fn applies Unitary complex matrix m(8x9) to register reg on 3 qubits specified in id0 id1 id2\n
*/
void ApplyTriQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1, int id2);

/*!
 *\fn applies Toffolli gate |x,y,z>->|x,y,z xor xy>\n
 *|1 0 0 0 0 0 0 0|
 *|0 1 0 0 0 0 0 0|
 *|0 0 1 0 0 0 0 0|
 *|0 0 0 1 0 0 0 0|
 *|0 0 0 0 1 0 0 0|
 *|0 0 0 0 0 1 0 0|
 *|0 0 0 0 0 0 0 1|
 *|0 0 0 0 0 0 1 0|
*/
void ApplyToffoli(IQRegister &reg, int id0, int id1, int id2);

/*!
 *\fn faster version of ApplyToffoli\n
*/
void ApplyFToffoli(IQRegister &reg, int id0, int id1, int id2);


/*!
 *\fn applies CNOT gate to reg |x,y> ->|x, x xor y> with toffolli and not
 *creates the computational basis\n
 *|1 0 0 0|\n
 *|0 1 0 0|\n
 *|0 0 0 1|\n
 *|0 0 1 0|\n
*/
void ApplyCnot(IQRegister &reg, int id0, int id1);

/*!
 *\fn faster version of ApplyCnot
*/
void ApplyFcnot(IQRegister &reg, int id0, int id1);
/*!
 *\fn applies Controlled Rotation gate to reg\n
 *|1 0 0 0|\n
 *|0 1 0 0|\n
 *|0 0 1 0|\n
 *|0 0 0 exp(i * alpha)|\n
*/
void ApplyCRot(IQRegister &reg, int id1, int id2, double alpha);

/*!
 *\fn applies Hadamard gate to reg\n
 *|1 -1|\n
 *|1  1|  * sqrt(1/2.0)\n
*/
void ApplyHadamard(IQRegister &reg, int id1);

/*!
 *\fn applies Not gate to reg\n
 *|0 1|\n
 *|1 0|\n
*/
void ApplyNot(IQRegister &reg, int id);

/*!
 *\fn applies Swap gate to reg\n
 *|1 0 0 0|\n
 *|0 0 1 0|\n
 *|0 1 0 0|\n
 *|0 0 0 1|\n
*/

void ApplySWAP(IQRegister &reg, int id0, int id1);


/*!
 *\fn applies Controlled Swap gate to reg\n
 *|1 0 0 0|\n
 *|0 0 1 0|\n
 *|0 1 0 0|\n
 *|0 0 0 1|\n
*/

void ApplyCSWAP(IQRegister &reg, int id0, int id1, int id2);

#endif // COMMON_H
