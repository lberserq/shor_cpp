#include <iostream>

#include <iface/infra/icommonworld.h>
#include <common/quantum/common.h>
#include <common/quantum_common_impl.h>
#include <common/config.h>
#include <vector>
#if (__cplusplus >= 201103L)
#include <unordered_map>
#endif
#include <algorithm>
#include <set>
#include <mpi.h>
#include <common/qmath.h>

complex_t QMath::i = complex_t(0, 1);

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
/*void log_init_reg(int m_w) {
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
}*/

ParallelSubSystemHelper::mpicfg ParallelSubSystemHelper::getConfig() {
    static ParallelSubSystemHelper::mpicfg cfg;
    static bool isDirty = true;

    if (!isDirty)
        return cfg;

    MPI_Comm_rank(MPI_COMM_WORLD, &cfg.rank);
    MPI_Comm_size(MPI_COMM_WORLD, &cfg.size);
    isDirty = false;
    return cfg;
}

bool ParallelSubSystemHelper::isInited()
{
    if (gWorld.get() && gWorld->GetGatesProvider()) {
        return (dynamic_cast<MPIGatesImpl *>(gWorld->GetGatesProvider()) != NULL);
    }
    return false;
}

void ParallelSubSystemHelper::sync::barrier()
{
    if (isInited()) {
        MPI_Barrier(MPI_COMM_WORLD);
    }
}


void ApplyCRot(IQRegister &reg, int id1, int id2, double alpha)
{

    mcomplex mult = std::exp(mcomplex(0, alpha));
    QMatrix m(4, 4);
    m(0, 0) = m(1, 1) = m(2, 2) = 1.0f;
    m(1, 2) = m(2, 1) = m(1, 3) = m(1, 3) = m(2, 3) = m(3, 2) = 0.0f;
    m(3, 3) = mult;
    ApplyDiQbitMatrix(m, reg, id1, id2);
}


void ApplyHadamard(IQRegister &reg, int id1)
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

void ApplyNot(IQRegister &reg, int id) {
    QMatrix m(2, 2);
    m(0, 0) = m(1, 1) = mcomplex(0, 0);
    m(1, 0) = m(0, 1) = mcomplex(1, 0);
    ApplyQbitMatrix(m, reg, id);
}

//naive realization not fast at all
void ApplyCnot(IQRegister &reg, int id0, int id1) {
    QMatrix m(4, 4);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m(i, j) = mcomplex(0, 0);
        }
    }
    m(0, 0) = m(1, 1) = m(2, 3) = m(3, 2) = mcomplex(1, 0);
    ApplyDiQbitMatrix(m, reg, id0, id1);
}


void ApplyFcnot(IQRegister &/*reg*/, int /*id0*/, int /*id1*/) {
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
    for (unsigned i = 0; i < static_cast<state_t>(1 << w); i += 2) {
        for (unsigned j = 0; j < static_cast<state_t>(1 << w); j += 2) {
            if (i == j) {
                long double val = errorLevel * xGenDrand();
                res(i, j) = cos(val);
                res(i + 1, j + 1) = res(i, j);
                res(i, j + 1) = sin(val);
                res(i + 1, j) = -res(i, j + 1);
            }
        }
    }
    return res;
}



//naive realization not fast at all
void ApplyToffoli(IQRegister &reg, int id0, int id1, int id2) {
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




void ApplyFToffoli(IQRegister &/*reg*/, int /*id0*/, int /*id1*/, int /*id2*/)
{
//        int sz = reg.getStates().size();
//        for (int i = 0; i < sz; i++){
//            state st = (reg.getStates())[i];
//            int st_id0 = (st & static_cast<state>(1 << id0));
//            int st_id1 = (st & static_cast<state>(1 << id1));
//            if (st_id0 != 0 && st_id1 != 0) {
//                st ^= static_cast<state>(1 << id2);
//                reg.getStates()[i] = st;
//            }

//        }
}


void ApplyQbitMatrix(const QMatrix &m, IQRegister &reg, int id0)
{


    QMatrix mEfficient = m;
    if (gWorld->GetNoiseProvider()->GetOperatorNoise()) {
        mEfficient = gWorld->GetNoiseProvider()->GetOperatorNoise()->GenNoisyMatrix(m);
    }

    gWorld->GetGatesProvider()->ApplyQbitMatrix(mEfficient, reg, id0);

    if (reg.getRepresentation() != REG_REPRESENTATION) {
        gWorld->GetGatesProvider()->ApplyQbitMatrix(mEfficient.conj(), reg,
                                                    id0 + reg.getWidth());
    }

    if (gWorld->GetNoiseProvider()->GetDensityMatrixNoise()
            && reg.getRepresentation() == MATRIX_REPRESENTATION) {
        gWorld->GetNoiseProvider()->GetDensityMatrixNoise()->ApplyNoiseForDensityMatrix(reg);
    }
}

void ApplyDiQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1)
{

    QMatrix mEfficient = m;
    if (gWorld->GetNoiseProvider()->GetOperatorNoise()) {
        mEfficient = gWorld->GetNoiseProvider()->GetOperatorNoise()->GenNoisyMatrix(m);
    }


    gWorld->GetGatesProvider()->ApplyDiQbitMatrix(mEfficient, reg, id0, id1);

    if (reg.getRepresentation() != REG_REPRESENTATION) {
        gWorld->GetGatesProvider()->ApplyDiQbitMatrix(mEfficient.conj(), reg,
                                                      id0 + reg.getWidth(),
                                                      id1 + reg.getWidth());
    }

    if (gWorld->GetNoiseProvider()->GetDensityMatrixNoise()
            && reg.getRepresentation() == MATRIX_REPRESENTATION) {
        gWorld->GetNoiseProvider()->GetDensityMatrixNoise()->ApplyNoiseForDensityMatrix(reg);
    }
}


void ApplyTriQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1, int id2)
{
    QMatrix mEfficient = m;
    if (gWorld->GetNoiseProvider()->GetOperatorNoise()) {
        mEfficient = gWorld->GetNoiseProvider()->GetOperatorNoise()->GenNoisyMatrix(m);
    }


    gWorld->GetGatesProvider()->ApplyTriQbitMatrix(mEfficient, reg,  id0, id1, id2);

    if (reg.getRepresentation() != REG_REPRESENTATION) {
        gWorld->GetGatesProvider()->ApplyTriQbitMatrix(mEfficient.conj(), reg,
                                                      id0 + reg.getWidth(),
                                                      id1 + reg.getWidth(),
                                                      id2 + reg.getWidth());
    }

    if (gWorld->GetNoiseProvider()->GetDensityMatrixNoise()
            && reg.getRepresentation() == MATRIX_REPRESENTATION) {
        gWorld->GetNoiseProvider()->GetDensityMatrixNoise()->ApplyNoiseForDensityMatrix(reg);
    }
}


void ApplySWAP(IQRegister &reg, int id0, int id1) {

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

}

void ApplyCSWAP(IQRegister &reg, int id0, int id1, int id2) {

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

}

