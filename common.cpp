#include <iostream>
#include "common.h"
#include "config.h"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <set>
/*!
  intent draw operators like a tex
  example
            -
          |>*
            -
          |>
 * \brief m_text
 */

static std::vector<std::string> m_text;
void log_init_reg(int m_w) {
    m_text.clear();
    m_text.resize(2 * m_w + 1);
    for (unsigned i = 1; i < m_text.size(); i += 2) {
        m_text[i]= "|>";
    }
    for (unsigned i = 0; i < m_text.size(); i += 2) {
        m_text[i]="  ";
    }
}

void log_reinit_reg(int new_mw) {
    int old_size = m_text.size();
    m_text.resize(2 * new_mw + 1);
    int reg_prelen = m_text[0].length();
    for (unsigned i = old_size; i < m_text.size(); i++) {
        m_text[i] += (i & 1) ? "|>" : "  ";
        for (int k = 0; k < reg_prelen - 2; k++) {
            m_text[i] += (i & 1) ? '-' : ' ';
        }
    }
}

void log_str(const std::set<int> &m_inputs, const char smb = '#') {
    int m_sz = m_text.size();
    if (m_inputs.size() > 3 || m_inputs.size() == 1) {
        int q_len = m_inputs.size() + 1;

        int first = -1, last = -1;
        for (int i = 0;  i < m_sz; i++) {
            bool prev_f = (i & 1) && m_inputs.find((i - 1) / 2 - 1) != m_inputs.end();
            bool curr_f = (i & 1) && m_inputs.find((i - 1) / 2) != m_inputs.end(); // build front face of operator
            if (curr_f) {
                m_text[i] += '|';
                for (int k = 0; k < q_len; k++) {
                    m_text[i] += ' ';
                }
                m_text[i] += '|';
                if (!prev_f) {
                    first = i;
                    m_text[i - 1] += ' ';
                    for (int k = 0; k < q_len; k++) {
                        m_text[i - 1] += '-';
                    }
                    m_text[i - 1] += ' ';
                }
                if (prev_f) {
                    m_text[i - 1] += " |";
                    for (int k = 0; k < q_len; k++) {
                        m_text[i - 1] += ' ';
                    }
                    m_text[i - 1] += '|';
                }

            } else if (!curr_f && prev_f) {
                last = i - 2;
                m_text[i - 1] += ' ';
                for (int k = 0; k < q_len; k++) {
                    m_text[i - 1] += '-';
                }
                m_text[i - 1] += ' ';
            }
        }
        if (last == -1) { //the operator connect with last wire
            m_text[m_sz - 1] += ' ';
            for (int k = 0; k < q_len; k++) {
                m_text[m_sz - 1] += '-';
            }
            m_text[m_sz - 1] += ' ';
            last = m_sz - 1;
        }

        for (int i = 0; i < m_sz; i++) {
            if (i & 1 && (i > last || i < first)) {
                for (int k = 0; k < q_len + 2; k++) {
                    m_text[i] += '-';
                }
            } else if (!(i & 1)  && (i > last + 1 || i < first - 1)) {
                for (int k = 0; k < q_len + 2; k++) {
                    m_text[i] += ' ';
                }
            }
        }

    } else{
        int imin = *m_inputs.begin() * 2 + 1;
        int imax = imin;
        int a[3];
        int cnt = 0;
        for (auto it = m_inputs.begin(); it != m_inputs.end(); it++) {
            int id = *it * 2 + 1;
            imin = std::min(id, imin);
            imax = std::max(id, imax);
            a[cnt++] = id;
        }

        for (int i = 0; i < m_sz; i++) {
            m_text[i] += (i & 1) ? '-'  : ' ';
        }

        for (int i = 0; i < m_sz; i++) {

            if (i == a[0] || (i == a[1] && cnt == 3)) {
                m_text[i] += 'x';
            } else  if (i > imin && i < imax) {
                m_text[i] += (i & 1) ? '|' : '|';
            } else if (cnt > 0 && i == a[cnt - 1]) {
                m_text[i] += smb;
            } else if (i < imin || i > imax) {
                m_text[i] += (i & 1) ? '-': ' ';
            }
        }

        for (int i = 0; i < m_sz; i++) {
            m_text[i] += (i & 1) ? '-'  : ' ';
        }
    }
    //log_print();
}


void log_print()
{
    for (auto it : m_text) {
        //std::fprintf(stderr, "%s\n",it.c_str());
        std::cerr << it << std::endl;
    }
}

int get_bit(state x, int id) {
    return (x >> id) & 1;
}

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
    std::set<int> m_s; m_s.insert(id1); m_s.insert(id2); log_str(m_s);

    mcomplex mult = std::exp(mcomplex(0, alpha));
    QMatrix m(4, 4);
    m(0, 0) = m(1, 1) = m(2, 2) = 1.0f;
    m(1, 2) = m(2, 1) = m(1, 3) = m(1, 3) = m(2, 3) = m(3, 2) = 0.0f;
    m(3, 3) = mult;
    ApplyDiQbitMatrix(m, reg, id1, id2);
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
    //    int sz = reg.getStates().size();
    //    for (int  i = 0; i < sz; i++) {
    //        state st = (reg.getStates())[i];
    //        int st_id = (st & static_cast<state>(1 << id0));
    //        if (st_id != 0) {
    //            st ^= static_cast<state>(1 << id1);
    //            reg.getStates()[i] = st;
    //        }
    //    }

}

double gen_rand()
{
    double sum = 0.0f;
    //    drand48_data st;
    //    srand48_r(0, &st);
    for (int i = 0; i < 12; ++i) {
        //double x; drand48_r(&st, &x);
        double x = drand48();
        sum += x;
    }
    return sum - 6.0f;
}

double errorLevel = 0.001f;

QMatrix genNoise(int w) {
    QMatrix res(1 << w, 1 << w);
    long double e_w = std::pow(M_SQRT1_2l, w - 1);
    for (unsigned i = 0; i < static_cast<state>(1 << w); i += 2) {
        for (unsigned j = 0; j < static_cast<state>(1 << w); j += 2) {
            if (i == j) {
                long double val = errorLevel * gen_rand();
                long double cs = cos(val), sn = sin(val);
                res(i, j) = cos(val);
                res(i + 1, j + 1) = res(i, j);
                res(i, j + 1) = sin(val);
                res(i + 1, j) = -res(i, j + 1);
            }
        }
    }
    //        fprintf(stderr, "\n");
    //    for (int i = 0; i < (1 << w); i++) {
    //        mcomplex sum = 0;
    //        for (int j = 0; j < (1 << w); j++) {
    //            mcomplex tmp = res(i, j);
    //            sum += tmp * tmp;
    //            double tre = tmp.real(), tim = tmp.imag();
    //            fprintf(stderr, "%.8llf + i%.8llf ", res(i, j).real(), res(i,j).imag());
    //        }
    //        fprintf(stderr, "PREC = %llf \n", sum.real());
    //        if (std::abs(sum.real() - 1.0) > errorLevel) {
    //            abort();
    //        }
    //    }

    //    static int p = 0;
    //    if (p++ == 5)
    //        abort();


    return res;
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
    //    int sz = reg.getStates().size();
    //    for (int i = 0; i < sz; i++){
    //        state st = (reg.getStates())[i];
    //        int st_id0 = (st & static_cast<state>(1 << id0));
    //        int st_id1 = (st & static_cast<state>(1 << id1));
    //        if (st_id0 != 0 && st_id1 != 0) {
    //            st ^= static_cast<state>(1 << id2);
    //            reg.getStates()[i] = st;
    //        }

    //    }
}


void ApplyQbitMatrix(const QMatrix &m, Qregister &reg, int id0)
{
    QMatrix resm = m;
#ifdef USE_NOISE
    resm = resm * genNoise(1);
#endif
#ifdef LOGGING
    std::set<int> m_s; m_s.insert(id0); log_str(m_s);
#endif
    std::vector<mcomplex> ampls;
    ampls.resize(reg.getAmpls().size());
    state st_sz = reg.getSize();
    int m_w = reg.getWidth();
    //int local_id0 = m_w - id0 - 1;
    int local_id0 = id0;
    //#pragma omp parallel for schedule(static)
    for (state i = 0; i < st_sz; i++) {
        int id_curr = get_bit(i, local_id0);
        ampls[i] = resm(id_curr, 0) * reg.getAmpls()[set_bit(i, id0, 0)] + resm(id_curr, 1) * reg.getAmpls()[set_bit(i, id0, 1)];
    }
    reg.setAmpls(ampls);
}


void ApplyDiQbitMatrix(const QMatrix &m, Qregister &reg, int id0,int id1)
{
    QMatrix resm = m;
#ifdef FULL_NOISE
    resm = resm * genNoise(2);
#endif

#ifdef LOGGING
    std::set<int> m_s; m_s.insert(id0); m_s.insert(id1); log_str(m_s);
#endif

    std::vector<mcomplex> ampls;
    ampls.resize(reg.getAmpls().size());
    state st_sz = reg.getSize();
    //    int m_w = reg.getWidth();
    //    int local_id0 = m_w - id0 - 1, local_id1 = m_w - id1 - 1;
    int local_id0 = id0;
    int local_id1 = id1;
    //#pragma omp parallel for schedule(static)
    for (state i = 0; i < st_sz; i++) {
        int id_curr0 = get_bit(i, local_id0);
        int id_curr1 = get_bit(i, local_id1);
        mcomplex ampl = 0.0f;
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                int s_id = set_bit(i, local_id0, j);
                s_id = set_bit(s_id, local_id1, k);
                ampl += resm(j * 2 + k, id_curr0 * 2 + id_curr1) * reg.getAmpls()[s_id];
            }
        }
        ampls[i] = ampl;
    }
    reg.setAmpls(ampls);

}


void ApplyTriQbitMatrix(const QMatrix &m, Qregister &reg, int id0, int id1, int id2)
{
    QMatrix resm = m;
#ifdef FULL_NOISE
    resm = resm * genNoise(3);
#endif

#ifdef LOGGING
    std::set<int> m_s; m_s.insert(id0);  m_s.insert(id1); m_s.insert(id2); log_str(m_s);
#endif

    std::vector<mcomplex> ampls;
    ampls.resize(reg.getAmpls().size());
    state st_sz = reg.getSize();
    //    int m_w = reg.getWidth();
    //    int local_id0 = m_w - id0 - 1;
    //    int local_id1 = m_w - id1 - 1;
    //    int local_id2 = m_w - id2 - 1;
    int local_id0 = id0;
    int local_id1 = id1;
    int local_id2 = id2;
    #pragma omp parallel for schedule(static)
    for (state i = 0; i < st_sz; i++) {
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
                    ampl += resm(j * 4 + k * 2 + p, id_curr0 * 4 + id_curr1 * 2 + id_curr2) * reg.getAmpls()[s_id];
                }
            }
        }
        ampls[i] = ampl;
    }
    reg.setAmpls(ampls);

}




void ApplySWAP(Qregister &reg, int id0, int id1) {

    QMatrix m(4, 4);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m(i, j) = 0.0f;
        }
    }
    m(0, 0) = 1.0f;
    m(3, 3) = 1.0f;
    m(1, 2) = m(2, 1) = 1.0f;
    ApplyDiQbitMatrix(m, reg, id0, id1);

    //    std::set<int> m_s; m_s.insert(id0); m_s.insert(id1);log_str(m_s, 'S');
    //    for (int i = 0; i < reg.getSize(); i++) {
    //        state st = reg.getStates()[i];
    //        int st_id0 = ((st & static_cast<state>(1 << id0)) != 0);
    //        int st_id1 = ((st & static_cast<state>(1 << id1)) != 0);
    //        st = set_bit(st, id0, st_id1);
    //        st = set_bit(st, id1, st_id0);
    //        reg.getStates()[i] = st;
    //    }

}

void ApplyCSWAP(Qregister &reg, int id0, int id1, int id2) {

    QMatrix m(8, 8);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            m(i,j) = (i == j && i < 4) ? 1.0f : 0.0f;
        }
    }
    m(4, 4) = 1.0f;
    m(7, 7) = 1.0f;
    m(5, 6) = m(6, 5) = 1.0f;
    ApplyTriQbitMatrix(m, reg, id0, id1, id2);

    //    std::set<int> m_s; m_s.insert(id0); m_s.insert(id1); m_s.insert(id2); log_str(m_s, 'C');
    //    for (int i = 0; i < reg.getSize(); i++) {
    //        state st = reg.getStates()[i];
    //        int st_id0 = ((st & static_cast<state>(1 << id0)) != 0);
    //        int st_id1 = ((st & static_cast<state>(1 << id1)) != 0);
    //        int st_id2 = ((st & static_cast<state>(1 << id2)) != 0);
    //        if (st_id0) {
    //            st = set_bit(st, id1, st_id2);
    //            st = set_bit(st, id2, st_id1);
    //            reg.getStates()[i] = st;
    //        }
    //    }
}

