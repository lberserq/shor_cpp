#include "qregister.h"
#include "common.h"
#include <cstdio>
#include <math.h>
#include <assert.h>
using namespace std;

IQRegister::~IQRegister() {

}

StaticQRegister::StaticQRegister(int width, state startState, const mcomplex &amplitude): m_width(width)
{
    m_states.resize(static_cast<state>(1 << width));
    m_states[startState] = amplitude;
}

size_t StaticQRegister::getWidth() const {
    return m_width;
}



std::vector<mcomplex> & StaticQRegister::getStates() {
    return m_states;
}

size_t StaticQRegister::getStatesSize() const {
    return m_states.size();
}

void StaticQRegister::setStates(const std::vector<mcomplex> &v)
{
    //assert(v.size() == (1 << m_width));
    m_states = v;
}


void StaticQRegister::allocSharedMem(int width) {
    m_width += width;
    m_states.resize(static_cast<state>(1 << m_width));
    int m_sz = m_states.size();
    for (int i = m_sz - 1; i >= 0; i--) {
        if (std::abs(m_states[i]) > g_eps) {
            m_states[i << width] = m_states[i];
            if (i) {
                m_states[i] = 0.0f;
            }
        }
    }
}

std::vector<mcomplex> StaticQRegister::getAllReg() {
    return m_states;
}

void StaticQRegister::collapseState(int id, long double amplProb) {
    double sum = 0.0f;
    int size = 0;
    bool flag = std::abs(amplProb) > g_eps;
    state m_sz = m_states.size();
    for (state i = 0; i < m_sz; i++) {
        if (std::abs(m_states[i]) > g_eps) {
            state st = i;
            int state_id1 = ((st & (state)(1 << id)) != 0);
            if (state_id1 == flag) {
                mcomplex p = m_states[i];
                size++;
                sum += std::pow(std::abs(p), 2);
            }
        }
    }
    std::vector<state> new_st;
    std::vector<mcomplex> new_ampls;
    new_st.resize(size);
    new_ampls.resize(size);
    m_width--;

    int cnt = 0;
    for(state i = 0; i < m_sz; i++)
    {
        if (std::abs(m_states[i]) > g_eps) {
            state st = i;
            int state_id1 = ((st & (state)(1 << id)) != 0);
            if(state_id1 == flag)
            {
                state right_part = 0;
                state left_part = 0;
                mcomplex amplitude = m_states[i];

                for (int j = 0; j < id; j++) {
                    right_part |= static_cast<state>(1 << j);
                }
                right_part &= st;

                for (int j = sizeof(state) * 8 - 1; j > id; j--) {
                    left_part |= static_cast<state>(1 << j);
                }
                left_part &= st;
                new_st[cnt] = (left_part >> 1) | right_part;

                new_ampls[cnt++] = (1.0 / std::sqrt(sum))* amplitude;
            }
        }
    }
    m_states.clear();
    m_states.resize(1 << m_width);
    for (int i = 0; i < size; i++) {
        m_states[new_st[i]] = new_ampls[i];
    }
}


void StaticQRegister::print()
{
    MPI_Barrier(MPI_COMM_WORLD);
    state m_sz = m_states.size();
    for(state i = 0; i < m_sz; i++)
    {
        //mcomplex t = ampl[i];
        //        if (std::abs(ampl[i]) < g_eps) {
        //            continue;
        //        }
        float f = std::pow(std::abs(m_states[i]), 2);
        fprintf(stderr, "%lf + %lfi|%lli> (%e) (|", m_states[i].real(),
                m_states[i].imag(), i,
                f);
        for(int j= m_width - 1; j >=0; j--)
        {
            if(j % 4 == 3)
                fprintf(stderr, " ");
            fprintf(stderr, "%i", (static_cast<state>(( 1 << j) & i) > 0));
        }

        fprintf(stderr, ">)\n");
    }

    fprintf(stderr, "\n");
}


double StaticQRegister::getLocalNorm() const {
    double sum = 0.0;
    size_t m_sz = m_states.size();
    for (state i = 0; i < m_sz; i++) {
        sum += std::abs(m_states[i]) * std::abs(m_states[i]);
    }
    return sum;
}

void StaticQRegister::printNorm() {

    double sum = getLocalNorm();
    fprintf(stderr, "NORM == %.8lf\n", sum);
}

size_t StaticQRegister::getOffset() const {
    return 0;
}


SharedQSimpleQRegister::SharedQSimpleQRegister(int width, state startState, const mcomplex &amplitude): m_global_width(width) {
    m_cfg = ParallelSubSystemHelper::getConfig();
    m_localstates.resize(static_cast<state>(1 << width) / m_cfg.size);
    m_local_size = m_localstates.size();
    m_local_offset = m_cfg.rank *  m_localstates.size();
    if (startState >= m_local_offset
            && startState < m_local_offset + m_localstates.size()) {
        m_localstates[startState - m_local_offset] = amplitude;
    }
}

size_t SharedQSimpleQRegister::getWidth() const {
    return m_global_width;
}

std::vector<mcomplex> & SharedQSimpleQRegister::getStates() {
    return m_localstates;
}

void SharedQSimpleQRegister::setStates(const std::vector<mcomplex> &v) {
    m_localstates = v;
}

size_t SharedQSimpleQRegister::getStatesSize() const {
    return m_localstates.size();
}

size_t SharedQSimpleQRegister::getOffset() const {
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

void SharedQSimpleQRegister::allocSharedMem(int width) {
    m_global_width += width;
    m_localstates.resize(static_cast<state>(1 << m_global_width) / m_cfg.size);
    for (int i = m_localstates.size() - 1; i >= 0; i--) {
        state curr_state = i + m_local_offset;
        if (i && std::abs(m_localstates[i]) > g_eps) {
            m_localstates[i << width] = m_localstates[i];
            m_localstates[i] = 0;
        }
    }
    m_local_offset = m_localstates.size() * m_cfg.rank;
    m_local_size = m_localstates.size();
}

std::vector<mcomplex> SharedQSimpleQRegister::getAllReg() {
    std::vector<mcomplex> fullReg; fullReg.resize(1);
    if (!ParallelSubSystemHelper::getConfig().rank) {
        fullReg.resize(static_cast<state>(1 << m_global_width));
    }
    //MPI_Barrier(MPI_COMM_WORLD);
    //    if (ParallelSubSystemHelper::getConfig().rank == 1) {
    //        std::cerr << "Finally0 " << m_localstates[0] << std::endl;
    //        std::cerr << "LOCAL_SIZE " << m_localstates.size() << std::endl;
    //    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(&m_localstates[0],
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

void SharedQSimpleQRegister::print() {
    std::vector<mcomplex> fullReg = getAllReg();
    if (!ParallelSubSystemHelper::getConfig().rank)
    {
        fprintf(stderr, "PP11\n");
        state m_sz = fullReg.size();
        for(state i = 0; i < m_sz; i++)
        {
            mcomplex t = fullReg[i];
                    if (std::abs(fullReg[i]) < g_eps) {
                        continue;
                    }
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

double SharedQSimpleQRegister::getLocalNorm() const {
    double sum = 0.0;
    size_t m_sz = m_localstates.size();
    for (state i = 0; i < m_sz; i++) {
        sum += std::abs(m_localstates[i]) * std::abs(m_localstates[i]);
    }
    return sum;
}

void SharedQSimpleQRegister::printNorm() {
    //std::vector<mcomplex> fullReg = getAllReg();
    double localNorm = getLocalNorm();
    double globalNorm = 0.0;
    MPI_Reduce(&localNorm, &globalNorm, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (!ParallelSubSystemHelper::getConfig().rank) {
        fprintf(stderr, "NORM == %.8lf\n", globalNorm);
    }
}


void SharedQSimpleQRegister::collapseState(int id, long double amplProb) {
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
    m_global_width--;
    m_global_width = (m_global_width < 0) ? 0 : m_global_width;
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
    const int drank = 2;

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


namespace QRegHelpers {



    static void DeleteVar(IQRegister &reg, int id)
    {
        long double probs = 0.0f;
        state m_sz = reg.getStates().size();
        for (state i = 0; i < m_sz; i++) {
            if (std::abs(reg.getStates()[i]) > g_eps) {
                state st = i + reg.getOffset();
                int state_id = ((st & static_cast<state>(1 << id)) != 0);
                if (!state_id) {
                    mcomplex amp = reg.getStates()[i];
                    probs += std::pow(std::abs(amp), 2);
                }
            }
        }
        long double gprobs = 0.0;
        MPI_Allreduce(&probs, &gprobs, 1, MPI_LONG_DOUBLE, MPI_SUM,
                      MPI_COMM_WORLD);


        probs = gprobs;

        //long double rnd_num =  static_cast<long double>(rand()) / RAND_MAX;
        long double rnd_num = xGenRand();
        MPI_Bcast(&rnd_num, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD);
        long double targ_val = 0.0f;
        if (rnd_num> probs) {
            targ_val = 1.0f;
        }
        reg.collapseState(id, targ_val);
    }

    void DeleteLocalVars(IQRegister &reg, int size)
    {
        int drank = 0;
        for (int i = 0; i < size; i++) {
            dumpVar(i, drank)
            reg.print();
            DeleteVar(reg, 0);
            reg.print();

        }
    }

    void SwapXY(IQRegister &reg, int width)
    {
        for (int i = 0; i < width / 2; i++) {
            ApplySWAP(reg, i, width - i - 1);
        }
    }

    void RegSwapLR(int width, IQRegister &in)
    {
        if (width > in.getWidth() / 2) {
            throw std::invalid_argument("Invalid width for RegSwapLR");
        }
        if (ParallelSubSystemHelper::getConfig().size == 1 && false) {
                    std::vector<state> news;
                    std::vector<mcomplex> ampls;
                    size_t m_sz = in.getStatesSize();
                    for (state i  = 0; i < m_sz; i++) {
                        if (std::abs(in.getStates()[i]) > g_eps) {
                            state left = i % static_cast<state>(1 << width);
                            state right = 0;
                            for (int j = 0; j < width; j++) {
                                right |= i & static_cast<state>(1 << (width + j));
                            }
                            state x = i ^ (left | right);
                            x |= static_cast<state>(left << width);
                            x |= (right >> width);
                            news.push_back(x);
                            ampls.push_back(in.getStates()[i]);
                        }
                    }

                    in.getStates().clear();
                    in.getStates().resize(m_sz);
                    m_sz = news.size();

                    for (state i = 0; i < m_sz; i++) {
                        in.getStates()[news[i]] = ampls[i];
                    }
        } else {
            for (int i = 0; i < width; i++) {
                ApplySWAP(in, i, i + width);
            }
        }

    }
}


