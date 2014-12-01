#include "multiplier.h"
#include "adder.h"
Multiplier::Multiplier(Qregister &in, int N, int width, int a, int ctl_id)
    :m_reg(in),
      m_N(N),
      m_width(width),
      m_a(a),
      m_ctlid(ctl_id)
{
}


void fix_local_vars(Qregister &in, int var_pin, int width) {
    for (int i = 0; i < width; i++) {
        ApplyToffoli(in, var_pin, width + i, 2 * width + 2 + i);
        ApplyToffoli(in, var_pin, 2 * width + 2 + i, width + i);
        ApplyToffoli(in, var_pin, width + i, 2 * width + 2 + i);
    }
}

Qregister& Multiplier::perform()
{

    mul();
    ////reg_swap(m_width, m_reg);
    ///
    fix_local_vars(m_reg, m_ctlid, m_width);

    mulinv();

    return m_reg;
}


void Multiplier::mul()
{
    int tw = 2 * m_width + 1;

    ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2, tw);

    basic_multiplication(m_a % m_N);

    ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2, tw);

    for (int i = 1; i < m_width; i++) {

        ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2 + i, tw);

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


void Multiplier::mulinv()
{
    int tw = 2 * m_width + 1;
    m_a = abel_inverse(m_a, m_N);

    for (int i = m_width - 1; i > 0; i--) {
        ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2 + i, tw);

        Adder *add = new Adder(m_N - ((1 << i) * m_a) % m_N, m_N, m_width, m_reg);
        m_reg = add->perform();

        delete add;
        ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2 + i, tw);

    }

    ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2, tw);

    basic_multiplication(m_a % m_N);

    ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2, tw);
}


void Multiplier::basic_multiplication(int x) {
    int tw = 2 * m_width + 1;

    for (int i = m_width - 1; i >= 0; i--) {
        if ((x >> i) & 1) {
            //ApplyToffoli(m_reg, m_ctlid, 2 * m_width + 2, tw);
            ApplyToffoli(m_reg, 2 * m_width + 2, tw, m_width + i);
        }
    }
}



Qregister &expamodn(Qregister &in, int N, int x, int width_in, int local_width)
{
    ApplyNot(in, 2 * local_width + 2);
    for (int i = 1; i <= width_in; i++) {
        int pow = x % N;
        for (int j = 1; j < i; j++) {
            pow *= pow;
            pow %= N;
        }
        Multiplier *mp = new Multiplier(in, N, local_width, pow, 3  * local_width + 1 + i);
        in = mp->perform();
        delete mp;
    }
    return in;
}
