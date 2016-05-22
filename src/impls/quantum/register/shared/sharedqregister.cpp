#include "sharedqregister.h"
#include <cstdio>
#include <math.h>
#include <assert.h>


SharedSimpleQRegister::SharedSimpleQRegister(int width, state startState, const mcomplex &amplitude, representation_t mode):
    m_representation(mode),
    m_global_width(width)
{

    if (m_representation == MATRIX_REPRESENTATION) {
        m_global_width *= 2;
    }

    m_cfg = ParallelSubSystemHelper::getConfig();
    m_localstates.resize(static_cast<state>(1 << m_global_width) / m_cfg.size);
    m_local_size = m_localstates.size();
    m_local_offset = m_cfg.rank *  m_localstates.size();
    if (startState >= m_local_offset
            && startState < m_local_offset + m_localstates.size()) {
        m_localstates[startState - m_local_offset] = amplitude;
    }


//    uint_type other_id = startState + static_cast<uint_type>(1u << (m_global_width / 2));
//    if (m_representation == MATRIX_REPRESENTATION && other_id  >= m_local_offset
//            && other_id < m_local_offset + m_localstates.size()) {
//        m_localstates[other_id - m_local_offset] = amplitude;
//    }



}

size_t SharedSimpleQRegister::getWidth() const {
    return m_representation == REG_REPRESENTATION  ? m_global_width : m_global_width / 2;
}

std::vector<mcomplex> & SharedSimpleQRegister::getStates() {
    return m_localstates;
}

void SharedSimpleQRegister::setStates(const std::vector<mcomplex> &v) {
    m_localstates = v;
}

size_t SharedSimpleQRegister::getStatesSize() const {
    return m_localstates.size();
}

size_t SharedSimpleQRegister::getOffset() const {
    return ParallelSubSystemHelper::getConfig().rank * m_localstates.size();
}
/* 00 -a- p1
 * 01 -b- p2
 * 10 -c- p3
 * 11 -d- p4
 *
 * 000-a- p1
 * 001-0- p1
 * 010-b- p2
 * 011-0- p2
 * 100-c- p3
 * 101-0- p3
 * 110-d- p4
 * 111-d- p4
 *
 *
 */

void SharedSimpleQRegister::allocSharedMem(int width) {
    uint_type width_p = width * ((m_representation == REG_REPRESENTATION) ? 1 : 2);
    m_global_width += width_p;
    m_localstates.resize(static_cast<state>(1 << m_global_width) / m_cfg.size);
    for (int i = m_localstates.size() - 1; i >= 0; i--) {
        //state curr_state = i + m_local_offset;
        if (i && std::abs(m_localstates[i]) > g_eps) {
            m_localstates[i << width_p] = m_localstates[i];
            m_localstates[i] = 0;
        }
    }
    m_local_offset = m_localstates.size() * m_cfg.rank;
    m_local_size = m_localstates.size();
}

std::vector<mcomplex> SharedSimpleQRegister::getAllReg() const {
    std::vector<mcomplex> fullReg; fullReg.resize(1);
    if (!ParallelSubSystemHelper::getConfig().rank) {
        fullReg.resize(static_cast<state>(1 << m_global_width));
    }
    //MPI_Barrier(MPI_COMM_WORLD);
    //    if (ParallelSubSystemHelper::getConfig().rank == 1) {
    //        std::cerr << "Finally0 " << m_localstates[0] << std::endl;
    //        std::cerr << "LOCAL_SIZE " << m_localstates.size() << std::endl;
    //    }

    std::vector<mcomplex> local_copy = m_localstates;
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(&local_copy[0],
            m_localstates.size(),
            MPI_DOUBLE_COMPLEX,
            &fullReg[0],
            m_localstates.size(),
            MPI_DOUBLE_COMPLEX,
            0,
            MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    return fullReg;
}

void SharedSimpleQRegister::print() const{
    std::vector<mcomplex> fullReg = getAllReg();
    if (!ParallelSubSystemHelper::getConfig().rank)
    {
        fprintf(stderr, "PP11\n");
        state m_sz = fullReg.size();
        for(state i = 0; i < m_sz; i++)
        {
//                    if (std::abs(fullReg[i]) < g_eps) {
//                        continue;
//                    }
            float f = std::pow(std::abs(fullReg[i]), 2);
            fprintf(stderr, "%lf + %lfi|%lli> (%e) (|", fullReg[i].real(),
                    fullReg[i].imag(), i,
                    f);
            for(int j = m_global_width - 1; j >= 0; j--)
            {
                if(j % 4 == 3)
                    fprintf(stderr, " ");
                fprintf(stderr, "%i", (static_cast<state>((1 << j) & i) > 0));
            }

            fprintf(stderr, ">)\n");
        }

        fprintf(stderr, "\n");
    }
}

double SharedSimpleQRegister::getLocalNorm() const {
    double sum = 0.0;
    size_t m_sz = m_localstates.size();
    for (state i = 0; i < m_sz; i++) {
        sum += std::abs(m_localstates[i]) * std::abs(m_localstates[i]);
    }
    return sum;
}

void SharedSimpleQRegister::printNorm() const {
    //std::vector<mcomplex> fullReg = getAllReg();
    double localNorm = getLocalNorm();
    double globalNorm = 0.0;
    MPI_Reduce(&localNorm, &globalNorm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (!ParallelSubSystemHelper::getConfig().rank) {
        fprintf(stderr, "NORM == %.8lf\n", globalNorm);
    }
}

representation_t SharedSimpleQRegister::getRepresentation() const {
    return m_representation;
}


void SharedSimpleQRegister::collapseState(int id, long double amplProb) {
    long double sum = 0.0f;
    size_t size = 0;
    bool flag = std::abs(amplProb) < g_eps;
    state m_sz = m_localstates.size();
    for (state i = 0; i < m_sz; i++) {
        if (std::abs(m_localstates[i]) > g_eps) {
            state st = i + m_local_offset;
            int state_id1 = ((st & (state)(1 << id)) != 0);
            if (state_id1 == !flag) {
                mcomplex p = m_localstates[i];
                size++;
                sum += std::pow(std::abs(p), 2);
            }
        }
    }


    std::vector<state> new_st;
    std::vector<mcomplex> new_ampls;
    new_st.resize(size);
    new_ampls.resize(size);
    m_global_width = (m_global_width == 0) ? 0 : m_global_width - 1;
    long double gsum = 0;
    MPI_Allreduce(&sum, &gsum, 1, MPI_LONG_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    sum = gsum;
    state right_mask = 0, left_mask = 0;
    for (int j = 0; j < id; j++) {
        right_mask |= static_cast<state>(1 << j);
    }

    for (int j = sizeof(state) * 8 - 1; j > id; j--) {
        left_mask |= static_cast<state>(1 << j);
    }

    int cnt = 0;
    //const int drank = 2;

    for(state i = 0; i < m_sz; i++)
    {
        if (std::abs(m_localstates[i]) > g_eps) {
            state st = i + m_local_offset;
            int state_id1 = ((st & (state)(1 << id)) != 0);
            if(state_id1 == !flag)
            {
                state right_part = 0;
                state left_part = 0;
                mcomplex amplitude = m_localstates[i];

                left_part = st & left_mask;
                right_part = st & right_mask;
                new_st[cnt] = (left_part >> 1) | right_part;

                new_ampls[cnt++] = static_cast<double>(1.0 / std::sqrt(sum)) * amplitude;
            }
        }
    }

    m_localstates.clear();
    m_localstates.resize(m_sz >> 1);
    m_local_offset = ParallelSubSystemHelper::getConfig().rank
                     * m_localstates.size();
    m_local_size = m_localstates.size();

    for (size_t i = 0; i < new_st.size(); i++) {
        m_localstates[new_st[i] - m_local_offset] = new_ampls[i];
    }
}





