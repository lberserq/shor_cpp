#ifndef QREGHELPER_H
#define QREGHELPER_H

namespace QRegHelper {
    void RegSwapLR(int width, IQRegister &in);
    void DeleteVar(IQRegister &reg, int size);
    void DeleteLocalVars(IQRegister &reg, int size);
    void SwapXY(IQRegister &reg, int width);
}
#endif // QREGHELPER_H

