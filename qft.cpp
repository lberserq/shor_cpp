#include "qft.h"
#include "common.h"

QFT::QFT()
{
}
//QFT -- scheme




void QFT::ApplyQft(Qregister &reg, int reg_width)
{
    for (int i = reg_width - 1; i >= 0; i--) {
        for (int j = reg_width - 1; j > i; j--) {
            double alpha = M_PI  / ((state)(1 << (j - i)));
            ApplyCRot(reg, j, i, alpha);
        }
        ApplyHadamard(reg, i);
    }
}
