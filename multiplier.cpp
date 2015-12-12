#include "multiplier.h"
#include "adder.h"
Multiplier::Multiplier(IQRegister &in, int N, int width, int a, int ctl_id)
    :m_reg(in),
      m_N(N),
      m_a(a),
      m_width(width),
      m_ctlid(ctl_id)
{
}


void fix_local_vars(IQRegister &in, int var_pin, int width) {
    for (int i = 0; i < width; i++) {
        //        ApplyToffoli(in, var_pin, width + i, 2 * width + 2 + i);
        //        ApplyToffoli(in, var_pin, 2 * width + 2 + i, width + i);
        //        ApplyToffoli(in, var_pin, width + i, 2 * width + 2 + i);
        ApplyCSWAP(in, var_pin, 2 * width + 2 + i, width + i);
    }
}

IQRegister& Multiplier::perform()
{

    mul();
    fix_local_vars(m_reg, m_ctlid, m_width);

    mulinv();

    return m_reg;
}

/*!
 *\fn
 *
 *built multiplicand in register
 *
 *represent m_a as a sum of a numbers x * y = x * (yi * (1 << i)
 *and add m_a * (1 << i) % N and sum this into a first part of a register
 *and sum this in register
 *
 */

void Multiplier::mul()
{
    int tw = 2 * m_width + 1; // local var for multiplier

    ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2, tw);

    BasicMultiplication(m_a % m_N);

    ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2, tw);

    for (int i = 1; i < m_width; i++) {

        q_log("MUL");
        q_log(i);
        ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2 + i, tw);

        //(1 << i) *(1 >> i) == 1
        Adder *add = new Adder(((1 << i) * m_a) % m_N, m_N, m_width, m_reg);
        m_reg = add->perform();
        delete add;


        ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2 + i, tw);

    }
}
/*!
 *\brief Calculates the a^-1 in this Field
 *If N is not prime not for all a a^-1 exists
 */

int abel_inverse(int a, int N) {
    int i = 1;
    while ( (i * a) % N != 1) {
        i++;
    }
    return i;
}

/*!
 *\fn
 *
 *inversive version of mul
 *
 */

void Multiplier::mulinv()
{
    int tw = 2 * m_width + 1; // first local var
    m_a = abel_inverse(m_a, m_N);

    for (int i = m_width - 1; i > 0; i--) {

        q_log("MULINV");
        q_log(i);
        ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2 + i, tw);

        Adder *add = new Adder(m_N - ((1 << i) * m_a) % m_N, m_N, m_width, m_reg);
        m_reg = add->perform();

        delete add;
        ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2 + i, tw);

    }

    ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2, tw);

    BasicMultiplication(m_a % m_N);

    ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2, tw);
}

/*!
 *\fn search bits in x
 *  m_width + i qbit contains m_reg(first width qbits) * (1 << i)
 *
 */
void Multiplier::BasicMultiplication(int x) {
    int tw = 2 * m_width + 1;

    for (int i = m_width - 1; i >= 0; i--) {
        if ((x >> i) & 1) { // i
            ApplyToffoli(m_reg, 2 * m_width + 2, tw, m_width + i); //m_width +i = m_reg * (1 << i) * indc(
        }
    }
}



IQRegister &expamodn(IQRegister &in, int N, int x, int width_in, int local_width)
{
    q_log("EAN");
    ApplyNot(in, 2 * local_width + 2);
    q_log("NOT");
    for (int i = 0; i < width_in; i++) {
        int pow = x % N;
        for (int j = 1; j < i + 1; j++) {
            pow *= pow;
            pow %= N;
        }
        Multiplier *mp = new Multiplier(in, N, local_width, pow, 3  * local_width + i + 2);
        in = mp->perform();
        //in.printNorm();
        q_log("EAXN");
        q_log(i);
        delete mp;
    }
    return in;
}
