#include "unitary_noise.h"
#include <common/quantum/common.h>
#include <iface/infra/icommonworld.h>
#include <common/infra/parallelsubsystemcommon.h>
#include <common/infra/xmlparsers.h>
#include "../noise_random.h"


QMatrix UnitaryNoiseImpl::GenNoisyMatrix(const QMatrix &m)
{
    const double errorLevel = 0.1f;
    int width = m.getRowsCount();
    QMatrix res(width, width);
    for (unsigned i = 0; i < static_cast<state_t>(width); i += 2) {
        for (unsigned j = 0; j < static_cast<state_t>(width); j += 2) {
            if (i == j) {
                long double val = errorLevel * xGenNRand();
                res(i, j) = cos(val);
                res(i + 1, j + 1) = res(i, j);
                res(i, j + 1) = sin(val);
                res(i + 1, j) = -res(i, j + 1);
            }
        }
    }
    return res * m;
}
