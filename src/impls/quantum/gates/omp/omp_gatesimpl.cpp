#include "omp_gatesimpl.h"
#include <iostream>
#include <algorithm>
#include <iface/infra/icommonworld.h>
void OMPGatesImpl::ApplyQbitMatrix(const QMatrix &m, IQRegister &reg, int id0)
{
    QMatrix resm = m;
#ifdef USE_NOISE
//    resm = resm * genNoise(1);
    if (gWorld->GetNoiseProvider()->GetOperatorNoise()) {
        resm = gWorld->GetNoiseProvider()->GetOperatorNoise()->GenNoisyMatrix(m);
    }
#endif
#ifdef LOGGING
    std::set<int> m_s; m_s.insert(id0); log_str(m_s);
#endif
    std::vector<mcomplex> ampls(reg.getStates().size(), 0);
    state_t st_sz = reg.getStatesSize();
    int local_id0 = id0;
    int alpha = (reg.getRepresentation() == REG_REPRESENTATION) ? 1 : 2;
    if (local_id0 >= alpha * static_cast<int>(reg.getWidth())) {
        throw std::invalid_argument("Invalid id for QBitOp");
    }

#pragma omp parallel for schedule(static)
    for (state_t i = 0; i < st_sz; i++) {
        int s_id = get_bit(i, local_id0);
        ampls[i] = resm(s_id, 0) * reg.getStates()[set_bit(i, id0, 0)] + resm(s_id, 1) * reg.getStates()[set_bit(i, id0, 1)];
    }
    reg.setStates(ampls);


}

void OMPGatesImpl::ApplyDiQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1)
{
    QMatrix resm = m;
#ifdef FULL_NOISE
    if (gWorld->GetNoiseProvider()->GetOperatorNoise()) {
        resm = gWorld->GetNoiseProvider()->GetOperatorNoise()->GenNoisyMatrix(m);
    }
#endif

#ifdef LOGGING
    std::set<int> m_s; m_s.insert(id0); m_s.insert(id1); log_str(m_s);
#endif

    std::vector<mcomplex> ampls(reg.getStates().size(), 0);
    state_t st_sz = reg.getStatesSize();
    int local_id0 = id0;
    int local_id1 = id1;

    int alpha = (reg.getRepresentation() == REG_REPRESENTATION) ? 1 : 2;
    if (local_id0 >= alpha * static_cast<int>(reg.getWidth())
            || local_id1 >= alpha * static_cast<int>(reg.getWidth())) {
        throw std::invalid_argument("Invalid id for DiQBitOp");
    }

#pragma omp parallel for schedule(static)
    for (state_t i = 0; i < st_sz; i++) {
        int id_curr0 = get_bit(i, local_id0);
        int id_curr1 = get_bit(i, local_id1);
        mcomplex ampl = 0.0f;
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                int s_id = set_bit(i, local_id0, j);
                s_id = set_bit(s_id, local_id1, k);
                ampl += resm(j * 2 + k, id_curr0 * 2 + id_curr1) * reg.getStates()[s_id];
            }
        }
        ampls[i] = ampl;
    }
    reg.setStates(ampls);
}

void OMPGatesImpl::ApplyTriQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1, int id2)
{
    QMatrix resm = m;
#ifdef FULL_NOISE
    if (gWorld->GetNoiseProvider()->GetOperatorNoise()) {
        resm = gWorld->GetNoiseProvider()->GetOperatorNoise()->GenNoisyMatrix(m);
    }
#endif

#ifdef LOGGING
    std::set<int> m_s; m_s.insert(id0);  m_s.insert(id1); m_s.insert(id2); log_str(m_s);
#endif

    std::vector<mcomplex> ampls(reg.getStates().size(), 0);
    state_t st_sz = reg.getStatesSize();
    int local_id0 = id0;
    int local_id1 = id1;
    int local_id2 = id2;

    int alpha = (reg.getRepresentation() == REG_REPRESENTATION) ? 1 : 2;
    if (local_id0 >= alpha * static_cast<int>(reg.getWidth())
            || local_id1 >= alpha * static_cast<int>(reg.getWidth())
            || local_id2 >= alpha * static_cast<int>(reg.getWidth())) {
        throw std::invalid_argument("Invalid id for TriQBitOp");
    }

#pragma omp parallel for schedule(static)
    for (state_t i = 0; i < st_sz; i++) {
        int id_curr0 = get_bit(i, local_id0);
        int id_curr1 = get_bit(i, local_id1);
        int id_curr2 = get_bit(i, local_id2);
        mcomplex ampl = 0.0;
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                for (int p = 0; p < 2; p++) {
                    int s_id = set_bit(i, local_id0, j);
                    s_id = set_bit(s_id, local_id1, k);
                    s_id = set_bit(s_id, local_id2, p);
                    ampl += resm(j * 4 + k * 2 + p, id_curr0 * 4 + id_curr1 * 2 + id_curr2) * reg.getStates()[s_id];
                }
            }
        }
        ampls[i] = ampl;
    }
    reg.setStates(ampls);
}
