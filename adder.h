#ifndef ADDER_H
#define ADDER_H
#include "qregister.h"
//adder inspired by ideas  in libquantum adder
/*!
 * \brief The Adder class
 *perform addition\n
 *main idea use cnot to add the bit
 *if (result is 1 or 2) (entangle and set the carry bits and  continue)
 *else do nothing
 *
 *adder contains of width - 1 full_adders and one half_adder
 *full adder entangles and use carry_qbits(to out)
 *half_adder only us carry_in
 *find_xlt set up the inverse x by modulo n and check the vars
 */


class Adder
{
    int m_N, m_a;
    IQRegister &m_reg;
    int m_width;
    void full_adder(int x, int dest_pin, int carry_in, int carry_out, int xlt);
    void full_adder_inv(int x, int dest_pin, int carry_in, int carry_out, int xlt);

    //half adders dont know about the carry
    void half_adder(int x, int dest_pin, int carry_pin, int xlt);
    void half_adder_inv(int x, int dest_pin, int carry_pin, int xlt);

    //in xlt we store the indicator of carry of the carrying//
    void find_xlt(int x);
    void gate_add(int x, int x_inv);
    void gate_add_inv(int x, int x_inv);
    void addition_n();
    void addition_inv();
public:
    Adder(int a, int n, int width, IQRegister &in);
    IQRegister &perform();
};

#endif // ADDER_H
