#include "qft_adder.h"
#include <qft.h>

void qft_adder::perform() {

    std::vector<int> idx;
    for (int i = 0; i < m_width / 2; i++) {
        idx.push_back(i);
    }
    QFT::ApplyQFT(m_reg, m_width, idx);
    //for (int i = 0; i <)
}
