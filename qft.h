#ifndef QFT_H
#define QFT_H
#include "qregister.h"
class QFT
{

public:
    QFT();
    /*!\brief
     *|x0>-^------^----^---H-|y3>
     *|x1>-|-^----|-^--|-H---|y2>
     *|x2>-|-|-^--|-|-H------|y1>
     *|x3>-|-|-|-H-----------|y0>
     *H -- Hadamard
     *^
     *| -- CROT(M_PI / 1 << |j - i|)
     */

    static void ApplyQFT(IQRegister &reg, int reg_width, const std::vector<int> &idx = std::vector<int>());
    static void ApplyQFTInv(IQRegister &reg, int reg_width, const std::vector<int> &idx = std::vector<int>());
};

#endif // QFT_H
