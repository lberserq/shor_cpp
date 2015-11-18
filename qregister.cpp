#include "qregister.h"
#include "common.h"
#include <cstdio>
#include <math.h>
#include <assert.h>
using namespace std;

IQRegister::~IQRegister() {

}

result_t IQRegister::FinalConstruct(int width, state StartState)
{

}



//void Qregister::printnorm() {
//    long double sum = 0.0;
//    for (state i = 0; i < this->getSize(); i++) {
//        sum += std::abs(this->getStates()[i]) * std::abs(this->getStates()[i]);
//    }
//    fprintf(stderr, "NORM == %.8llf\n", sum);
//}

//Qregister::Qregister(int width, state init_state)
//{
//    //    ampl.resize(1);
//    //    states.resize(1);
//    //    m_width = width;
//    //    ampl[0] = 1.0f;
//    //    states[0] = init_state;
//    //    log_init_reg(width);
//    ampl.resize(static_cast<state>(1 << width));
//    m_width = width;
//    ampl[init_state] = 1.0f;

//}
////one favourite hack to exchange the |l|mid|r| to |r|mid|l| //exchange bit part the
//void reg_swap(int width, Qregister &in)
//{
//    /*for (int i = 0; i < in.getSize(); i++) {
//        state cur_state = (in.getStates())[i];
//        state left = cur_state & (1 << width);
//        state right = 0;
//        for (int j = 0; j < width; j++) {
//            right |= cur_state & static_cast<state>(1 << (width + j));
//        }
//        state new_state = cur_state ^ (left | right);
//        new_state |= (left << width);
//        new_state |= (right >> width);
//        (in.getStates())[i] = new_state;
//    }*/
//    std::vector<state> news;
//    std::vector<mcomplex> ampls;
//    state m_sz = in.getSize();
//    for (state i  = 0; i < m_sz; i++) {
//        if (std::abs(in.getStates()[i]) > g_eps) {
//            state left = i % static_cast<state>(1 << width);
//            state right = 0;
//            for (int j = 0; j < width; j++) {
//                right |= i & static_cast<state>(1 << (width + j));
//            }
//            state x = i ^ (left | right);
//            x |= static_cast<state>(left << width);
//            x |= (right >> width);
//            news.push_back(x);
//            ampls.push_back(in.getStates()[i]);
//        }
//    }
//    in.getStates().clear();
//    in.getStates().resize(m_sz);
//    m_sz = news.size();

//    for (state i = 0; i < m_sz; i++) {
//        in.getStates()[news[i]] = ampls[i];
//    }
//}

//void Qregister::shrink_reg(int new_width)
//{
//    int delta = m_width - new_width;
//    m_width = new_width;
//    for (unsigned  i = 0; i < states.size(); i++) {
//        states[i] = static_cast<state>(states[i] >> delta);
//    }
//}

//void Qregister::alloc_smem(int size)
//{
//    m_width += size;
//    ampl.resize(1 << m_width);/*
//    for (unsigned i = 0; i < states.size();i++) {
//        states[i] = static_cast<state>(states[i] << size);
//    }*/
//    int m_sz = ampl.size();
//    ampl.resize(static_cast<state>(1 << m_width));
//    for (int i = m_sz - 1; i >= 0; i--) {
//        if (std::abs(ampl[i]) > g_eps) {
//            ampl[i << size] = ampl[i];
//            if (i) {
//                ampl[i] = 0.0f;
//            }
//        }
//    }
//    log_reinit_reg(m_width);
//}

//void Qregister::print()
//{
//    state m_sz = ampl.size();
//    for(state i = 0; i < m_sz; i++)
//    {
//        //mcomplex t = ampl[i];
////        if (std::abs(ampl[i]) < g_eps) {
////            continue;
////        }
//        float f = std::pow(std::abs(ampl[i]), 2);
//        fprintf(stderr, "%llf + %llfi|%lli> (%e) (|", ampl[i].real(),
//                ampl[i].imag(), i,
//                f);
//        for(int j= m_width - 1; j >=0; j--)
//        {
//            if(j % 4 == 3)
//                fprintf(stderr, " ");
//            fprintf(stderr, "%i", (static_cast<state>(( 1 << j) & i) > 0));
//        }

//        fprintf(stderr, ">)\n");
//    }

//    fprintf(stderr, "\n");
//}

//void Qregister::collapse_state(int id, long double prob_amp)
//{
//    long double sum = 0.0f;
//    int size = 0;
//    bool flag = std::abs(prob_amp) < g_eps;
//    state m_sz = ampl.size();
//    for (state i = 0; i < m_sz; i++) {
//        if (std::abs(ampl[i]) > g_eps) {
//            state st = i;
//            int state_id1 = ((st & (state)(1 << id)) != 0);
//            if (state_id1 == !flag) {
//                mcomplex p = ampl[i];
//                size++;
//                sum += std::pow(std::abs(p), 2);
//            }
//        }
//    }
//    std::vector<state> new_st;
//    std::vector<mcomplex> new_ampls;
//    new_st.resize(size);
//    new_ampls.resize(size);
//    m_width--;

//    int cnt = 0;
//    for(state i = 0; i < m_sz; i++)
//    {
//        if (std::abs(ampl[i]) > g_eps) {
//            state st = i;
//            int state_id1 = ((st & (state)(1 << id)) != 0);
//            if(state_id1 == !flag)
//            {
//                state right_part = 0;
//                state left_part = 0;
//                mcomplex amplitude = ampl[i];

//                for (int j = 0; j < id; j++) {
//                    right_part |= static_cast<state>(1 << j);
//                }
//                right_part &= st;

//                for (int j = sizeof(state) * 8 - 1; j > id; j--) {
//                    left_part |= static_cast<state>(1 << j);
//                }
//                left_part &= st;
//                new_st[cnt] = (left_part >> 1) | right_part;

//                new_ampls[cnt++] = (1.0 / std::sqrt(sum))* amplitude;
//            }
//        }
//    }
//    ampl.clear();
//    ampl.resize(m_sz);
//    for (int i = 0; i < size; i++) {
//        ampl[new_st[i]] = new_ampls[i];
//    }
//}



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

void StaticQRegister::collapseState(int id, long double amplProb) {
    long double sum = 0.0f;
    int size = 0;
    bool flag = std::abs(amplProb) < g_eps;
    state m_sz = m_states.size();
    for (state i = 0; i < m_sz; i++) {
        if (std::abs(m_states[i]) > g_eps) {
            state st = i;
            int state_id1 = ((st & (state)(1 << id)) != 0);
            if (state_id1 == !flag) {
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
            if(state_id1 == !flag)
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
    m_states.resize(m_sz);
    for (int i = 0; i < size; i++) {
        m_states[new_st[i]] = new_ampls[i];
    }
}


void StaticQRegister::print()
{
    state m_sz = m_states.size();
    for(state i = 0; i < m_sz; i++)
    {
        //mcomplex t = ampl[i];
        //        if (std::abs(ampl[i]) < g_eps) {
        //            continue;
        //        }
        float f = std::pow(std::abs(m_states[i]), 2);
        fprintf(stderr, "%llf + %llfi|%lli> (%e) (|", m_states[i].real(),
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

void StaticQRegister::printNorm() {
    long double sum = 0.0;
    size_t m_sz = m_states.size();
    for (state i = 0; i < m_sz; i++) {
        sum += std::abs(m_states[i]) * std::abs(m_states[i]);
    }
    fprintf(stderr, "NORM == %.8llf\n", sum);
}


static void delete_var(IQRegister &reg, int id)
{
    long double probs = 0.0f;
    state m_sz = reg.getStates().size();
    for (state i = 0; i < m_sz; i++) {
        if (std::abs(reg.getStates()[i]) > g_eps) {
            state st = i;
            int state_id = ((st & static_cast<state>(1 << id)) != 0);
            if (!state_id) {
                mcomplex amp = reg.getStates()[i];
                probs += std::pow(std::abs(amp), 2);
            }
        }
    }

    long double rnd_num = static_cast<long double>(rand()) / RAND_MAX;
    long double targ_val = 0.0f;
    if (rnd_num > probs) {
        targ_val = 1.0f;
    }
    reg.collapseState(id, targ_val);
}

void DeleteLocalVars(IQRegister &reg, int size)
{
    for (int i = 0; i < size; i++) {
        delete_var(reg, 0);
    }
}

void SwapXY(IQRegister &reg, int width)
{
    for (int i = 0; i < width / 2; i++) {
        ApplySWAP(reg, i, width - i - 1);

    }
}


//void reg_swap(int width, Qregister &in)
//{
//    /*for (int i = 0; i < in.getSize(); i++) {
//        state cur_state = (in.getStates())[i];
//        state left = cur_state & (1 << width);
//        state right = 0;
//        for (int j = 0; j < width; j++) {
//            right |= cur_state & static_cast<state>(1 << (width + j));
//        }
//        state new_state = cur_state ^ (left | right);
//        new_state |= (left << width);
//        new_state |= (right >> width);
//        (in.getStates())[i] = new_state;
//    }*/
//    std::vector<state> news;
//    std::vector<mcomplex> ampls;
//    state m_sz = in.getSize();
//    for (state i  = 0; i < m_sz; i++) {
//        if (std::abs(in.getStates()[i]) > g_eps) {
//            state left = i % static_cast<state>(1 << width);
//            state right = 0;
//            for (int j = 0; j < width; j++) {
//                right |= i & static_cast<state>(1 << (width + j));
//            }
//            state x = i ^ (left | right);
//            x |= static_cast<state>(left << width);
//            x |= (right >> width);
//            news.push_back(x);
//            ampls.push_back(in.getStates()[i]);
//        }
//    }
//    in.getStates().clear();
//    in.getStates().resize(m_sz);
//    m_sz = news.size();

//    for (state i = 0; i < m_sz; i++) {
//        in.getStates()[news[i]] = ampls[i];
//    }
//}

void RegSwapLR(int width, IQRegister &in)
{
    if (width > in.getWidth() / 2) {
        throw std::invalid_argument("Invalid width for RegSwapLR");
    }
    /*for (int i = 0; i < in.getSize(); i++) {
        state cur_state = (in.getStates())[i];
        state left = cur_state & (1 << width);
        state right = 0;
        for (int j = 0; j < width; j++) {
            right |= cur_state & static_cast<state>(1 << (width + j));
        }
        state new_state = cur_state ^ (left | right);
        new_state |= (left << width);
        new_state |= (right >> width);
        (in.getStates())[i] = new_state;
    }*/
    std::vector<state> news;
    std::vector<mcomplex> ampls;
    state m_sz = in.getStatesSize();
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
}
