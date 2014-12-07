#ifndef QFT_H
#define QFT_H
#include "qregister.h"
class QFT
{

public:
    QFT();
    /*! \brief
     *|x0>-^------^----^---H-|y3>\n
     *|x1>-|-^----|-^--|-H---|y2>\n
     *|x2>-|-|-^--|-|-H------|y1>\n
     *|x3>-|-|-|-H-----------|y0>\n
     *H -- Hadamard\n
     *^\n
     *| -- CROT(M_PI / 1 << |j - i|)\n
     */

    static void ApplyQft(Qregister &reg, int reg_width);
};

#endif // QFT_H
