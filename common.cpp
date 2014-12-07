
#include "common.h"
#include "config.h"
#include <vector>
#include <map>
#include <algorithm>
int set_bit(state x, unsigned char id, int val) {
    switch (val) {
    case 0:
        x &= ~(1 << id);
        break;
    case 1:
        x |= (1 << id);
        break;
    default:
        break;
    }
    return x;
}

void ApplyCRot(Qregister &reg, int id1, int id2, double alpha)
{

    mcomplex mult = std::exp(mcomplex(0, alpha));
    for (int i = 0; i < reg.getSize(); i++) {
        state st = reg.getStates()[i];
        int id1_bit = ((st & static_cast<state>(1 << id1)) != 0);
        int id2_bit = ((st & static_cast<state>(1 << id2)) != 0);
        if (id1_bit && id2_bit) {
            reg.getAmpls()[i] *= mult;
        }
    }
}


void ApplyHadamard(Qregister &reg, int id1)
{
    QMatrix m(2, 2);
    for (int i = 0 ; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            m(i, j) = mcomplex(M_SQRT1_2l, 0);
        }
    }
    m(1, 1) *= -1.0;
    ApplyQbitMatrix(m, reg, id1);
}

void ApplyNot(Qregister &reg, int id) {
    QMatrix m(2, 2);
    m(0, 0) = m(1, 1) = mcomplex(0, 0);
    m(1, 0) = m(0, 1) = mcomplex(1, 0);
    ApplyQbitMatrix(m, reg, id);
}

//naive realization not fast at all
void ApplyCnot(Qregister &reg, int id0, int id1) {
    QMatrix m(4, 4);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m(i, j) = mcomplex(0, 0);
        }
    }
    m(0, 0) = m(1, 1) = m(2, 3) = m(3, 2) = mcomplex(1, 0);
    ApplyDiQbitMatrix(m, reg, id0, id1);
}


void ApplyFcnot(Qregister &reg, int id0, int id1) {
    int sz = reg.getStates().size();
    for (int  i = 0; i < sz; i++) {
        state st = (reg.getStates())[i];
        int st_id = (st & static_cast<state>(1 << id0));
        if (st_id != 0) {
            st ^= static_cast<state>(1 << id1);
            reg.getStates()[i] = st;
        }
    }

}



//naive realization not fast at all
void ApplyToffoli(Qregister &reg, int id0, int id1, int id2) {
    QMatrix m(8, 8);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            m(i, j) = mcomplex(0, 0);
        }
    }
    for (int i = 0; i < 6; i++) {
        m(i, i) = mcomplex(1, 0);
    }
    m(6, 7) = m(7, 6) = mcomplex(1, 0);
    ApplyTriQbitMatrix(m, reg, id0, id1, id2);
}




void ApplyFToffoli(Qregister &reg, int id0, int id1, int id2)
{
    int sz = reg.getStates().size();
    for (int i = 0; i < sz; i++){
        state st = (reg.getStates())[i];
        int st_id0 = (st & static_cast<state>(1 << id0));
        int st_id1 = (st & static_cast<state>(1 << id1));
        if (st_id0 != 0 && st_id1 != 0) {
            st ^= static_cast<state>(1 << id2);
            reg.getStates()[i] = st;
        }

    }
}


void ApplyQbitMatrix(const QMatrix &m, Qregister &reg, int id0)
{
    std::vector<state> states;
    std::vector<mcomplex> ampls;
    std::map<state, int> used;
    for (int i = 0; i < reg.getSize(); i++) {
        state st = (reg.getStates())[i];
        int stateid0 = ((st & static_cast<state>(1 << id0)) != 0);
        mcomplex cur_ampl = (reg.getAmpls())[i];
        for (int j = 0; j < 2; j++) {
            mcomplex res_ampl = m(j, stateid0) * cur_ampl;
            if (std::abs(res_ampl) > g_eps) {
                state st_t = st;
                st_t = set_bit(st_t, id0, j);
                bool found = used.find(st_t) != used.end();
                if (!found) {
                    states.push_back(st_t);
                    ampls.push_back(res_ampl);
                    used[st_t] = states.size() - 1;
                } else {
                    ampls[used[st_t]] += res_ampl;
                }
            }
        }
    }

    std::vector<state> new_states;
    std::vector<mcomplex> new_ampls;
    for (unsigned i = 0; i < ampls.size(); i++) {
        if (std::abs(ampls[i]) > g_eps) {
            new_states.push_back(states[i]);
            new_ampls.push_back(ampls[i]);
        }
    }


    reg.setAmpls(new_ampls);
    reg.setStates(new_states);
}


void ApplyDiQbitMatrix(const QMatrix &m, Qregister &reg, int id0,int id1)
{
    std::vector<state> states;
    std::vector<mcomplex> ampls;
    std::map<state, int> used;
    for (int i = 0; i < reg.getSize(); i++) {
        int st = (reg.getStates())[i];
        int stateid0 = ((st & static_cast<state>(1 << id0)) != 0);
        int stateid1 = ((st & static_cast<state>(1 << id1)) != 0);
        mcomplex cur_ampl = (reg.getAmpls())[i];
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                mcomplex res_ampl = m(j * 2 + k, stateid0 * 2 + stateid1) * cur_ampl;
                if (std::abs(res_ampl) > g_eps) {
                    state st_t = st;
                    st_t = set_bit(st_t, id0, j);
                    st_t = set_bit(st_t, id1, k);
                    bool found = used.find(st_t) != used.end();
                    if (!found) {
                        states.push_back(st_t);
                        ampls.push_back(res_ampl);
                        used[st_t] = states.size() - 1;
                    } else {
                        ampls[used[st_t]] += res_ampl;
                    }
                }
            }
        }
    }
    std::vector<state> new_states;
    std::vector<mcomplex> new_ampls;
    for (unsigned i = 0; i < ampls.size(); i++) {
        if (std::abs(ampls[i]) > g_eps) {
            new_states.push_back(states[i]);
            new_ampls.push_back(ampls[i]);
        }
    }


    reg.setAmpls(new_ampls);
    reg.setStates(new_states);
}


void ApplyTriQbitMatrix(const QMatrix &m, Qregister &reg, int id0, int id1, int id2)
{
    std::vector<state> states;
    std::vector<mcomplex> ampls;
    std::map<state, int> used;
    for (int i = 0; i < reg.getSize(); i++) {
        state st = (reg.getStates())[i];
        int stateid0 = ((st & static_cast<state>(1 << id0)) != 0);
        int stateid1 = ((st & static_cast<state>(1 << id1)) != 0);
        int stateid2 = ((st & static_cast<state>(1 << id2)) != 0);
        mcomplex cur_ampl = (reg.getAmpls())[i];
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                for (int l = 0; l < 2; l++) {
                    mcomplex res_ampl = m(j * 4 + k * 2 + l, stateid0 * 4 + stateid1 * 2 + stateid2) * cur_ampl;
                    if (std::abs(res_ampl) > g_eps) {
                        state st_t = st;
                        st_t = set_bit(st_t, id0, j);
                        st_t = set_bit(st_t, id1, k);
                        st_t = set_bit(st_t, id2, l);
                        bool found = used.find(st_t) != used.end();
                        if (!found) {
                            states.push_back(st_t);
                            ampls.push_back(res_ampl);
                            used[st_t] = states.size() - 1;
                        } else {
                            ampls[used[st_t]] += res_ampl;
                        }
                    }
                }
            }
        }
    }
    std::vector<state> new_states;
    std::vector<mcomplex> new_ampls;
    for (unsigned i = 0; i < ampls.size(); i++) {
        if (std::abs(ampls[i]) > g_eps) {
            new_states.push_back(states[i]);
            new_ampls.push_back(ampls[i]);
        }
    }


    reg.setAmpls(new_ampls);
    reg.setStates(new_states);
}




void ApplySWAP(Qregister &reg, int id0, int id1) {
    for (int i = 0; i < reg.getSize(); i++) {
        state st = reg.getStates()[i];
        int st_id0 = ((st & static_cast<state>(1 << id0)) != 0);
        int st_id1 = ((st & static_cast<state>(1 << id1)) != 0);
        st = set_bit(st, id0, st_id1);
        st = set_bit(st, id1, st_id0);
        reg.getStates()[i] = st;
    }
}

void ApplyCSWAP(Qregister &reg, int id0, int id1, int id2) {
    for (int i = 0; i < reg.getSize(); i++) {
        state st = reg.getStates()[i];
        int st_id0 = ((st & static_cast<state>(1 << id0)) != 0);
        int st_id1 = ((st & static_cast<state>(1 << id1)) != 0);
        int st_id2 = ((st & static_cast<state>(1 << id2)) != 0);
        if (st_id0) {
            st = set_bit(st, id1, st_id2);
            st = set_bit(st, id2, st_id1);
            reg.getStates()[i] = st;
        }
    }
}

