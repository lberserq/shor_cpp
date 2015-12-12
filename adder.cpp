#include "adder.h"
#include "qmatrix.h"
#include "common.h"
#include "config.h"

/*!
 * \brief Adder::Adder
 * \param a
 * \param n
 * \param width
 * \param in
 *adds a to register by modulo n
 */
Adder::Adder(int a, int n, int width, IQRegister &in):
    m_N(n),
    m_a(a),
    m_reg(in),
    m_width(width)

{
}

IQRegister & Adder::perform()
{
    addition_n();
    q_log("addn");
    addition_inv(); // disentangle with ancill
    q_log("addinv");
    return m_reg;
}


void Adder::addition_n()
{
    find_xlt(m_N - m_a);
    gate_add((1 << m_width) + m_a - m_N, m_a);
}


void Adder::addition_inv()
{
    ApplyCnot(m_reg, 2 * m_width + 1, 2 * m_width);
    gate_add_inv((1 << m_width) - m_a,  m_N - m_a);
    QRegHelpers::RegSwapLR(m_width, m_reg);
    find_xlt(m_a);
}

void Adder::gate_add(int x, int x_inv)
{
    //int tw = REG_NUM * m_width + 2; //total width  with local variables
    int id = 0;
    for (int i = 0; i < m_width - 1; i++) {
        if (x & (1 << i)) {
            //id = 1 << i;
            id = 2;
        } else {
            id = 0;
        }
        if (x_inv & (1 << i)) {
            id++;
        }
        full_adder(id, m_width + i, i, i + 1, 2 * m_width);
        q_log("GATE_ADD");
        q_log(i);
    }
    id = 0;
    if (x & (1 << (m_width - 1))) {
        id = 2;
    }
    if (x_inv & (1 << (m_width - 1))) {
        id++;
    }
    half_adder(id, 2 * m_width - 1, m_width - 1, 2 * m_width);
}


void Adder::gate_add_inv(int x, int x_inv)
{
    //int tw = REG_NUM * m_width + 2; //total width  with local variables
    int id = 0;

    id = 0;
    if (x & (1 << (m_width - 1))) {
        id = 2;
    }
    if (x_inv & (1 << (m_width - 1))) {
        id++;
    }
    half_adder_inv(id, m_width - 1, 2 * m_width - 1, 2 * m_width);

    for (int i = m_width - 2; i >= 0; i--) {
        if (x & (1 << i)) {
            //id = 1 << i;
            id = 2;
        } else {
            id = 0;
        }
        if (x_inv & (1 << i)) {
            id++;
        }
        full_adder_inv(id, i, m_width + i, m_width + i + 1, 2 * m_width);
        q_log("GATE_ADD_INV");
        q_log(i);
    }
}
//add x to dest carry in carry_pin xlt is a
//len xlt is a local variables-bits(ancilled)
void Adder::half_adder(int x, int dest_pin, int carry_pin, int xlt) {
    int ctl_id = 2 * m_width + 1;
    if (x == 0) {//add of 0 and now its 0
        ApplyCnot(m_reg, dest_pin, carry_pin);
    }
    if (x == 1) {//add of 0 and now its 1
        ApplyCnot(m_reg, dest_pin, carry_pin);
        ApplyToffoli(m_reg, ctl_id, xlt, carry_pin);
    }
    if (x == 2) {//add of 1 and now its 0 we need to save fact about it
        ApplyNot(m_reg, xlt);
        ApplyToffoli(m_reg, ctl_id, xlt, carry_pin);
        ApplyCnot(m_reg, dest_pin, carry_pin);
        ApplyNot(m_reg, xlt);
    }
    if (x == 3) {
        ApplyCnot(m_reg, ctl_id, carry_pin);
        ApplyCnot(m_reg, dest_pin, carry_pin);
    }
}

void Adder::half_adder_inv(int x, int dest_pin, int carry_pin, int xlt)
{
    int ctl_id = 2 * m_width + 1;
    if (x == 0) {//add of 0 and now its 0
        ApplyCnot(m_reg, dest_pin, carry_pin);
    }
    if (x == 1) {//add of 0 and now its 1
        ApplyCnot(m_reg, dest_pin, carry_pin);
        ApplyToffoli(m_reg, ctl_id, xlt, carry_pin);
    }
    if (x == 2) {//add of 1 and now its 0 we need to save fact about it
        ApplyNot(m_reg, xlt);
        ApplyCnot(m_reg, dest_pin, carry_pin);
        ApplyToffoli(m_reg, ctl_id, xlt, carry_pin);
        ApplyNot(m_reg, xlt);
    }
    if (x == 3) {
        ApplyCnot(m_reg, ctl_id, carry_pin);
        ApplyCnot(m_reg, dest_pin, carry_pin);
    }
}



void Adder::full_adder(int x, int dest_pin, int carry_in, int carry_out, int xlt)
{
    int ctl_id = 2 * m_width + 1;
    if (x == 0) {
        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);
        ApplyCnot(m_reg, dest_pin, carry_in);
    }


    if (x == 1) {
        ApplyToffoli(m_reg, ctl_id, xlt, dest_pin);
        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);

        ApplyToffoli(m_reg, ctl_id, xlt, dest_pin);
        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);

        ApplyToffoli(m_reg, ctl_id, xlt, carry_in);
        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);

        ApplyCnot(m_reg, dest_pin, carry_in);
    }

    if (x == 2) {
        ApplyNot(m_reg, xlt);

        ApplyToffoli(m_reg, ctl_id, xlt, dest_pin);
        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);

        ApplyToffoli(m_reg, ctl_id, xlt, dest_pin);
        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);

        ApplyToffoli(m_reg, ctl_id, xlt, carry_in);
        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);

        ApplyCnot(m_reg, dest_pin, carry_in);

        ApplyNot(m_reg, xlt);

    }

    if (x == 3) {
        ApplyToffoli(m_reg, ctl_id, carry_in, carry_out);
        ApplyCnot(m_reg, ctl_id, carry_in);
        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);
        ApplyCnot(m_reg, dest_pin, carry_in);
    }
}

void Adder::full_adder_inv(int x, int dest_pin, int carry_in, int carry_out, int xlt)
{
    int ctl_id = 2 * m_width + 1;

    if (x == 0) {
        ApplyCnot(m_reg, dest_pin, carry_in);
        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);
    }


    if (x == 1) {
        ApplyCnot(m_reg, dest_pin, carry_in);

        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);
        ApplyToffoli(m_reg, ctl_id, xlt, carry_in);

        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);
        ApplyToffoli(m_reg, ctl_id, xlt, dest_pin);

        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);
        ApplyToffoli(m_reg, ctl_id, xlt, dest_pin);
    }

    if (x == 2) {
        ApplyNot(m_reg, xlt);

        ApplyCnot(m_reg, dest_pin, carry_in);

        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);
        ApplyToffoli(m_reg, ctl_id, xlt, carry_in);

        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);
        ApplyToffoli(m_reg, ctl_id, xlt, dest_pin);

        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);
        ApplyToffoli(m_reg, ctl_id, xlt, dest_pin);

        ApplyNot(m_reg, xlt);

    }


    if (x == 3) {
        ApplyCnot(m_reg, dest_pin, carry_in);
        ApplyToffoli(m_reg, dest_pin, carry_in, carry_out);

        ApplyCnot(m_reg, ctl_id, carry_in);
        ApplyToffoli(m_reg, ctl_id, carry_in, carry_out);
    }
}
//apply
void Adder::find_xlt(int x)
{
    int ctl_id = 2 * m_width + 1;
    if (x & (1 << (m_width - 1))) {
        ApplyCnot(m_reg, 2 * m_width - 1, m_width - 1);
        ApplyNot(m_reg, 2 * m_width - 1);
        ApplyCnot(m_reg, 2 * m_width - 1, 0);
    } else {
        ApplyNot(m_reg, 2 * m_width - 1);
        ApplyCnot(m_reg, 2 * m_width - 1, m_width - 1);
    }

    for (int i = m_width - 2; i > 0; i--) {
        if (x & (1 << i)) {
           ApplyToffoli(m_reg, i + 1, m_width + i, i);
           ApplyNot(m_reg, m_width + i);
           ApplyToffoli(m_reg, i + 1, m_width + i, 0);
        } else {
            ApplyNot(m_reg, m_width + i);
            ApplyToffoli(m_reg, i + 1, m_width + i, i);
        }
    }

    if (x & 1) {
        ApplyNot(m_reg, m_width);
        ApplyToffoli(m_reg, m_width, 1, 0);
    }

    ApplyToffoli(m_reg, ctl_id, 0, 2 * m_width);
    if (x & 1) {
        ApplyToffoli(m_reg, m_width, 1, 0);
        ApplyNot(m_reg, m_width);
    }


    for (int i = 1; i <= m_width - 2; i++) {
        if (x & (1 << i)) {
           ApplyToffoli(m_reg, i + 1, m_width + i, 0);
           ApplyNot(m_reg, m_width + i);
           ApplyToffoli(m_reg, i + 1, m_width + i, i);
        } else {
            ApplyToffoli(m_reg, i + 1, m_width + i, i);
            ApplyNot(m_reg, m_width + i);
        }
    }

    if (x & (1 << (m_width - 1))) {
        ApplyCnot(m_reg, 2 * m_width - 1, 0);
        ApplyNot(m_reg, 2 * m_width - 1);
        ApplyCnot(m_reg, 2 * m_width - 1, m_width - 1);
    } else {
        ApplyCnot(m_reg, 2 * m_width - 1, m_width - 1);
        ApplyNot(m_reg, 2 * m_width - 1);
    }
}




