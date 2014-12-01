#ifndef ADDER_H
#define ADDER_H
#include "qregister.h"
class Adder
{
    int m_N, m_a;
    Qregister &m_reg;
    int m_width;
    void add_inv_mod(int width);
    void uxfull(int x, int dest_pin, int carry_in, int carry_out, int xlt, int Len, int total);
    void uxfullinv(int x, int dest_pin, int carry_in, int carry_out, int xlt, int Len, int total);

    //half adders dont know about the carry
    void uxhalf(int x, int dest_pin, int carry_pin, int xlt, int Len, int total);
    void uxhalfinv(int x, int dest_pin, int carry_pin, int xlt, int Len, int total);

    //in xlt we store the indicator of carry of the carrying
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
