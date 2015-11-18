#include "qft.h"
#include "common.h"

QFT::QFT()
{
}
//QFT -- scheme


#define ind_trans(i) (use_idx  ? idx[i] : i)

void QFT::ApplyQFT(Qregister &reg, int reg_width, const std::vector<int> &idx)
{
    bool use_idx = (idx.size() > 0);
    int st_val = (use_idx) ? idx.size() - 1 : reg_width - 1;

    for (int i = st_val; i >= 0; i--) {
        for (int j = st_val; j > i; j--) {

            double alpha = M_PI  / (static_cast<state>(1 << (j - i)));
            ApplyCRot(reg, ind_trans(j), ind_trans(i), alpha);
        }
        ApplyHadamard(reg, i);
    }
}


void QFT::ApplyQFTInv(Qregister &reg, int reg_width, const std::vector<int> &idx)
{
    bool use_idx = (idx.size() > 0);
    int st_val = (use_idx) ? idx.size() - 1 : reg_width - 1;

    for (int i = st_val - 1; i >= 0; i--) {
        for (int j = st_val - 1; j > i; j--) {
            double alpha = -M_PI  / (static_cast<state>(1 << (j - i)));
            ApplyCRot(reg, ind_trans(j), ind_trans(i), alpha);
        }
        ApplyHadamard(reg, i);
    }
}
