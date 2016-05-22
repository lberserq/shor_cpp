#include "noise.h"
#include <common/quantum/common.h>
#include <iface/infra/icommonworld.h>
#include <common/infra/parallelsubsystemcommon.h>
#include <common/infra/xml_parsers.h>

#if __cplusplus >= 201103L
#include <random>
#endif
namespace
{
#if __cplusplus < 201103L
    long  double xGenNRand() {
        const int N = 20;
        double sum = 0;
        for (int i = 0; i < N; i++) {
            sum += drand48();
        }
        return sum - N / 2;
    }
#else
    long double xGenNRand()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::normal_distribution<long double> d;
        return d(gen);
    }
#endif
}


QMatrix UnitaryNoiseImpl::GenNoisyMatrix(const QMatrix &m)
{
    const double errorLevel = 0.1f;
    int width = m.getRowsCount();
    QMatrix res(width, width);
    for (unsigned i = 0; i < static_cast<state>(width); i += 2) {
        for (unsigned j = 0; j < static_cast<state>(width); j += 2) {
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



CraussNoiseImpl::CraussNoiseImpl(const std::string &fileName)
{
    XmlParser parser(fileName.c_str());
    for (uint8_t  i = 0; i < CRAUSS_MAX_DIM_V1; i++) {
        std::vector<std::pair<QMatrix, std::vector<uint_type> > > res =  parser.GetOperators(i + 1);
        for (size_t j = 0; j < res.size(); j++) {
            m_CraussOps[i].push_back(res[j].first);
        }
    }

}

QMatrix CraussNoiseImpl::GenNoisyMatrix(const QMatrix &m)
{
    int width = m.getRowsCount();
    if (width & (width - 1)) {
        return m;
    }
    QMatrix res(width, width);
    int id = 0;
    while ((1 << id++) != width) {
        ;
    }
    if (id >= CRAUSS_MAX_DIM_V1) {
        return m;
    }
    for (size_t i = 0; i < m_CraussOps[id].size(); i++) {
        res = res +  m_CraussOps[id][i] * m * m_CraussOps[id][i].conj();
    }
    return res;
}


//#if __cplusplus < 201103L
//extern template ParallelSubSystemHelper::thread::parallelAddAssign<mcomplex>(mcomplex *, mcomplex *, size_t);
//#else
//extern template void ParallelSubSystemHelper::thread::parallelAddAssign<mcomplex>(mcomplex *, mcomplex *, size_t);
//#endif


extern service_ptr_t<ICommonWorld> gWorld;

CraussNoiseDensityImpl::CraussNoiseDensityImpl(const std::string &fileName, int dim):
    m_dim(dim)
{
    XmlParser parser(fileName.c_str());
    m_CraussOps = parser.GetOperators(static_cast<uint8_t>(dim));
}



extern bool isFin;


void CraussNoiseDensityImpl::ApplyNoiseForDensityMatrix(IQRegister &reg)
{
    if (!isFin)
        return;
    //IGates * gatesProvider = gWorld->GetGatesProvider();
    size_t width = reg.getWidth() * 2;
    if (!m_CraussOps.size())
    {
        const double errorLevel = 0.05;
        long double val  = errorLevel * xGenNRand();
        mcomplex p = std::exp(-val * val);

        size_t len = (1 << m_dim);
        QMatrix qq(len, len);

        for (size_t  i = 0; i < len - 1; ++i) {
            qq(i, i) = 1.0;
        }

        qq(len - 1, len - 1) = std::sqrt(1.0-p);
        std::vector<uint_type> vct;
        m_CraussOps.push_back(std::make_pair(qq, vct));

        for (size_t  i = 0; i < len - 1; ++i) {
            qq(i, i) = 0.0;
        }
        qq(len - 1, len - 1) = std::sqrt(p);
        m_CraussOps.push_back(std::make_pair(qq, vct));

    }
    std::vector<mcomplex> localRegStates(reg.getStatesSize());

    for (size_t i = 0; i < m_CraussOps.size(); i++) {
        int id0 = 1, id1 = 1, id2 = 2;
        if (m_CraussOps[i].second.size()) {
            id0 = m_CraussOps[i].second[0];
        }

        if (m_CraussOps[i].second.size() > 1) {
            id1 = m_CraussOps[i].second[1];
        }

        if (m_CraussOps[i].second.size() > 2) {
            id2 = m_CraussOps[i].second[2];
        }
        QMatrix mp = m_CraussOps[i].first.conj();
        std::vector<mcomplex> currentReg;

        switch (m_dim)
        {
        case 1:
            gWorld->GetGatesProvider()->ApplyQbitMatrix(m_CraussOps[i].first.conj(), reg, id0);
            gWorld->GetGatesProvider()->ApplyQbitMatrix(m_CraussOps[i].first, reg, width / 2 + id0);
            currentReg = reg.getAllReg();
            ParallelSubSystemHelper::thread::parallelAddAssign(&localRegStates[0], &currentReg[0], localRegStates.size());
            break;
        case 2:
            gWorld->GetGatesProvider()->ApplyDiQbitMatrix(m_CraussOps[i].first, reg, id0, id1);
            gWorld->GetGatesProvider()->ApplyDiQbitMatrix(m_CraussOps[i].first.conj(), reg, width / 2 + id0, width / 2 + id1);
            currentReg = reg.getAllReg();
            ParallelSubSystemHelper::thread::parallelAddAssign(&localRegStates[0], &currentReg[0], localRegStates.size());
            break;
        case 3:
            gWorld->GetGatesProvider()->ApplyTriQbitMatrix(m_CraussOps[i].first.conj(), reg, id0, id1, id2);
            gWorld->GetGatesProvider()->ApplyTriQbitMatrix(m_CraussOps[i].first, reg, width / 2 + id0, width / 2 + id1, width / 2 + id2);
            currentReg = reg.getAllReg();
            ParallelSubSystemHelper::thread::parallelAddAssign(&localRegStates[0], &currentReg[0], localRegStates.size());
            break;
        default:
            break;
        }
    }
    reg.setStates(localRegStates);
}
