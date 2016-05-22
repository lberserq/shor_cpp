#ifndef MULTIPLIER_H
#define MULTIPLIER_H
#include <iface/quantum/iqregister.h>
/*!
 *\brief Class Multiplier Multiply the val in  Register in with the width width on a % n used
 *ctl_id as a local variable\n
 *
 *how it works \n
 *x * y = (x * 2 ^ p0 + x * 2 ^p1 + ... x * 2^pt\n
 *where is  2^p0 + 2^p1 + ... + 2^pt == y\n
 *basic_multiplication creates in the local_variables_size x * 2 ^ pi
 *then mull calls adder to sum this values
 *
 *
 */
class Multiplier
{
    IQRegister &m_reg;
    int m_N;
    int m_a;
    int m_width;
    int m_ctlid;
    void mul();
    void mulinv();

    void BasicMultiplication(int x);

public:
    Multiplier(IQRegister &in, int N, int width, int a, int ctl_id);
    IQRegister &perform();
};
/*!
 *\fn Main part of shor algorithm realize the Uf gate\n
 * calculates x ^ (reg_val) % N\n
 * width_in is a actual width of reg which store reg_val\n
 * local_width -- with of local_variables\n
 * Realized by width_in multiplications
 */
IQRegister &expamodn(IQRegister &in, int N, int x, int width_in, int local_width);

#endif // MULTIPLIER_H
