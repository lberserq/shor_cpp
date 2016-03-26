#include "noise.h"
#include "xml_parsers.h"
#include "common.h"
#include "icommonworld.h"


QMatrix UnitaryNoiseImpl::GenNoisyMatrix(const QMatrix &m)
{
    const double errorLevel = 0.001f;
    int width = m.getRowsCount();
    QMatrix res(width, width);
    for (unsigned i = 0; i < static_cast<state>(width); i += 2) {
        for (unsigned j = 0; j < static_cast<state>(width); j += 2) {
            if (i == j) {
                long double val = errorLevel * xGenDrand();
                res(i, j) = cos(val);
                res(i + 1, j + 1) = res(i, j);
                res(i, j + 1) = sin(val);
                res(i + 1, j) = -res(i, j + 1);
            }
        }
    }
    return res * m;
}


CraussNoiseImpl::CraussNoiseImpl(const std::string &fileName)
{
    XmlParser parser(fileName.c_str());
    for (uint8_t  i = 0; i < CRAUSS_MAX_DIM_V1; i++) {
        m_CraussOps[i] = parser.GetOperators(i + 1);
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
extern ICommonWorld *gWorld;

CraussNoiseDensityImpl::CraussNoiseDensityImpl(const std::string &fileName, int dim):
    m_dim(dim)
{
    XmlParser parser(fileName.c_str());
    m_CraussOps = parser.GetOperators(static_cast<uint8_t>(dim));
}

void CraussNoiseDensityImpl::ApplyNoiseForDensityMatrix(IQRegister &reg)
{
    //IGates * gatesProvider = gWorld->GetGatesProvider();
    size_t width = reg.getWidth();

    for (size_t i = 0; i < m_CraussOps.size(); i++) {
        switch (m_dim)
        {
        case 1:
            ApplyQbitMatrix(m_CraussOps[i], reg, 0);
            ApplyQbitMatrix(m_CraussOps[i].conj(), reg, width / 2);
            break;
        case 2:
            ApplyDiQbitMatrix(m_CraussOps[i], reg, 0, 1);
            ApplyDiQbitMatrix(m_CraussOps[i].conj(), reg, width / 2, width / 2 + 1);
            break;
        case 3:
            ApplyTriQbitMatrix(m_CraussOps[i], reg, 0, 1, 2);
            ApplyTriQbitMatrix(m_CraussOps[i].conj(), reg, width / 2, width / 2 + 1, width / 2 + 2);
            break;
        default:
            //ApplyNQbitMatrix(m_CraussOps[i], dim, 0);
            //ApplyNQbitMatrix(m_CraussOps[i].conj(), dim, width / 2);
            break;
        }
    }
}
