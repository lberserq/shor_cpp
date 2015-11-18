#ifndef QFT_ADDER_H
#define QFT_ADDER_H
#include <qregister.h>

class qft_adder
{
    int m_N, m_a;
    Qregister &m_reg;
    int m_width;
public:

    qft_adder(Qregister reg, int w, int n, int a) : m_N(n), m_a(a), m_reg(reg), m_width(w) {}
    void perform();
};

#endif // QFT_ADDER_H
