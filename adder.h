#ifndef ADDER_H
#define ADDER_H
#include "qregister.h"
//adder inspired by ideas  in libquantum(Copyright Hendrik Weimer, Björn Butscher) adder
//License of libquantum is GPL V3

/*!
 * \brief The Adder class required a lot of mem.
 *perform addition\n
 *main idea use cnot to add the bit\n
 *if (result is 1 or 2) (entangle and set the carry bits and  continue)\n
 *else do nothing\n
 *adder contains of width - 1 full_adders and one half_adder\n
 *full adder entangles and use carry_qbits(to out)\n
 *(new carry sets in carry out\n
 *half_adder only us carry_in\n
 *find_xlt set up the inverse x by modulo n and check the vars\n
 */


class Adder
{
    int m_N, m_a;
    Qregister &m_reg;
    int m_width;
    void full_adder(int x, int dest_pin, int carry_in, int carry_out, int xlt);
    void inverse_full_adder(int x, int dest_pin, int carry_in, int carry_out, int xlt);

    //half adders dont know about the carry
    void half_adder(int x, int dest_pin, int carry_pin, int xlt);
    void inverse_half_adder(int x, int dest_pin, int carry_pin, int xlt);

    //in xlt we store the indicator of carry of the carrying//
    void find_xlt(int x);
    void gate_add(int x, int x_inv);
    void gate_add_inv(int x, int x_inv);
    void addition_n();
    void addition_inv();
public:
    Adder(int a, int n, int width, Qregister &in);
    Qregister &perform();
};

#endif // ADDER_H
