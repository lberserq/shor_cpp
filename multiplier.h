#ifndef MULTIPLIER_H
#define MULTIPLIER_H
#include "common.h"

/*!
 *\brief Class Multiplier Multiply the val in  Register in with the width width on a % n used
 *ctl_id as a local variable\n
 *
 *how it works \n
 *x * y = (x * 2 ^ p0 + x * 2 ^p1 + ... x * 2^pt\n
 *where is  2^p0 + 2^p1 + ... + 2^pt == y\n
 *basic_multiplication creates in the local_variables_size x * 2 ^ i\n
 *then mull calls adder to sum this values
 *

 */
class Multiplier
{
    Qregister &m_reg;
    int m_N;
    int m_a;
    int m_width;
    int m_ctlid;
    void mul();
    void mulinverse();

    void basic_multiplication(int x);

public:
    Multiplier(Qregister &in, int N, int width, int a, int ctl_id);
    Qregister &perform();
};
/*!
 *\fn Main part of shor algorithm realize the Uf gate\n
 * calculates \param x ^ (reg_val) %  \param N\n
 * width_in is a actual width of reg which store reg_val\n
 * local_width -- width of local_variables\n
 * Realized by \param val_width multiplications
 */
Qregister &expamodn(Qregister &in, int N, int x, int val_width, int local_width);

#endif // MULTIPLIER_H
