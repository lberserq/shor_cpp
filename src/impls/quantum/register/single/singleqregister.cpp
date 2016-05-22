#include "singleqregister.h"
#include <cstdio>
#include <math.h>
#include <assert.h>


SingleQRegister::SingleQRegister(int width, state startState, const mcomplex &amplitude, representation_t mode):
    m_width(width),
    m_representation(mode)
{
    if (m_representation == MATRIX_REPRESENTATION) {
        m_width *= 2;
    }
    m_states.resize(static_cast<state>(1ll << m_width));
    m_states[startState] = amplitude;
//    if (m_representation == MATRIX_REPRESENTATION) {
//        m_states[startState + m_width / 2] = amplitude;
//    }
}

size_t SingleQRegister::getWidth() const {
    return m_representation == REG_REPRESENTATION  ? m_width : m_width / 2;
}



std::vector<mcomplex> & SingleQRegister::getStates() {
    return m_states;
}

size_t SingleQRegister::getStatesSize() const {
    return m_states.size();
}

void SingleQRegister::setStates(const std::vector<mcomplex> &v)
{
    //assert(v.size() == (1 << m_width));
    m_states = v;
}


void SingleQRegister::allocSharedMem(int width) {
    if (m_representation == MATRIX_REPRESENTATION) {
        m_width += width * 2;
    } else {
        m_width += width;
    }
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

std::vector<mcomplex> SingleQRegister::getAllReg() const {
    return m_states;
}

void SingleQRegister::collapseState(int id, long double amplProb) {
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


representation_t SingleQRegister::getRepresentation() const {
    return m_representation;
}

void SingleQRegister::print() const
{
    ParallelSubSystemHelper::barrier();
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


double SingleQRegister::getLocalNorm() const {
    double sum = 0.0;
    size_t m_sz = m_states.size();
    for (state i = 0; i < m_sz; i++) {
        sum += std::abs(m_states[i]) * std::abs(m_states[i]);
    }
    return sum;
}

void SingleQRegister::printNorm() const {

    double sum = getLocalNorm();
    fprintf(stderr, "NORM == %.8lf\n", sum);
}

size_t SingleQRegister::getOffset() const {
    return 0;
}


