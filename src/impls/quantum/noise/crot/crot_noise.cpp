#include "crot_noise.h"
#include <common/quantum/common.h>
#include <iface/infra/icommonworld.h>
#include <common/infra/parallelsubsystemcommon.h>
#include <common/infra/xmlparsers.h>
#include "../noise_random.h"


QMatrix CRotNoiseImpl::GenNoisyMatrix(const QMatrix &m)
{
    double errorLevel = 0.1f;
    if (m.getColumnsCount() > 4) {
        return m;
    }

    double xi = xGenNRand();
    QMatrix res(m.getRowsCount(), m.getColumnsCount());
    if (m.getColumnsCount() == 2) {
        res(0, 0) = res(1, 1)  = cos(xi * errorLevel);
        res(1, 0) = - res(0, 1) = sin(xi * errorLevel);
    } else {

        res(0, 0) = res(1, 1) = res(2, 2) = 1.0;
        res(3, 3) = std::exp(-mcomplex(0, 1) * errorLevel * xi);
    }
    return m * res;
}
