#include <iostream>
#include "common.h"
#include "config.h"
#include <vector>
#if (__cplusplus >= 201103L)
#include <unordered_map>
#endif
#include <algorithm>
#include <set>
#include <mpi.h>

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
    ParallelSubSystemHelper::mpicfg cfg;
    MPI_Comm_rank(MPI_COMM_WORLD, &cfg.rank);
    MPI_Comm_size(MPI_COMM_WORLD, &cfg.size);
    return cfg;
}

void ApplyCRot(IQRegister &reg, int id1, int id2, double alpha)
{
    //std::set<int> m_s; m_s.insert(id1); m_s.insert(id2); log_str(m_s);

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
    //long double e_w = std::pow(M_SQRT1_2l, w - 1);
    for (unsigned i = 0; i < static_cast<state>(1 << w); i += 2) {
        for (unsigned j = 0; j < static_cast<state>(1 << w); j += 2) {
            if (i == j) {
                long double val = errorLevel * xGenDrand();
                //long double cs = cos(val), sn = sin(val);
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

/*namespace OpenMPQuantumOperations
{
    void ApplyQbitMatrix(const QMatrix &m, IQRegister &reg, int id0)
    {
        QMatrix resm = m;
#ifdef USE_NOISE
        resm = resm * genNoise(1);
#endif
#ifdef LOGGING
        std::set<int> m_s; m_s.insert(id0); log_str(m_s);
#endif
        std::vector<mcomplex> ampls;
        ampls.resize(reg.getStates().size());
        state st_sz = reg.getStatesSize();
        //int m_w = reg.getWidth();
        //int local_id0 = m_w - id0 - 1;
        int local_id0 = id0;
        if (local_id0 >= static_cast<int>(reg.getWidth())) {
            throw std::invalid_argument("Invalid id for OneQBitOp");
        }
#pragma omp parallel for schedule(static)
        for (state i = 0; i < st_sz; i++) {
            int s_id = get_bit(i, local_id0);
            ampls[i] = resm(s_id, 0) * reg.getStates()[set_bit(i, id0, 0)] + resm(s_id, 1) * reg.getStates()[set_bit(i, id0, 1)];
        }
        reg.setStates(ampls);
    }


    void ApplyDiQbitMatrix(const QMatrix &m, IQRegister &reg, int id0,int id1)
    {
        QMatrix resm = m;
#ifdef FULL_NOISE
        resm = resm * genNoise(2);
#endif

#ifdef LOGGING
        std::set<int> m_s; m_s.insert(id0); m_s.insert(id1); log_str(m_s);
#endif

        std::vector<mcomplex> ampls;
        ampls.resize(reg.getStates().size());
        state st_sz = reg.getStatesSize();
        int local_id0 = id0;
        int local_id1 = id1;

        if (local_id0 >= static_cast<int>(reg.getWidth())
                || local_id1 >= static_cast<int>(reg.getWidth())) {
            throw std::invalid_argument("Invalid id for DiQBitOp");
        }
#pragma omp parallel for schedule(static)
        for (state i = 0; i < st_sz; i++) {
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


    void ApplyTriQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1, int id2)
    {
        QMatrix resm = m;
#ifdef FULL_NOISE
        resm = resm * genNoise(3);
#endif

#ifdef LOGGING
        std::set<int> m_s; m_s.insert(id0);  m_s.insert(id1); m_s.insert(id2); log_str(m_s);
#endif

        std::vector<mcomplex> ampls;
        ampls.resize(reg.getStates().size());
        state st_sz = reg.getStatesSize();
        int local_id0 = id0;
        int local_id1 = id1;
        int local_id2 = id2;
        if (local_id0 >= static_cast<int>(reg.getWidth())
                || local_id1 >= static_cast<int>(reg.getWidth())
                || local_id2 >= static_cast<int>(reg.getWidth())) {
            throw std::invalid_argument("Invalid id for TriQBitOp");
        }
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
                        ampl += resm(j * 4 + k * 2 + p, id_curr0 * 4 + id_curr1 * 2 + id_curr2) * reg.getStates()[s_id];
                    }
                }
            }
            ampls[i] = ampl;
        }
        reg.setStates(ampls);

    }

}//end of OpenMPQuantumOperations
*/

void ApplyQbitMatrix(const QMatrix &m, IQRegister &reg, int id0)
{
/*    MPI_Barrier(MPI_COMM_WORLD);
    QMatrix resm = m;
#ifdef USE_NOISE
    resm = resm * genNoise(1);
#endif
#ifdef LOGGING
    std::set<int> m_s; m_s.insert(id0); log_str(m_s);
#endif
    std::vector<mcomplex> ampls;
    ampls.resize(reg.getStates().size());
    memset(&ampls[0], 0, ampls.size() * sizeof(ampls[0]));
    state st_sz = reg.getStatesSize();
    int local_id0 = id0;
    if (local_id0 >= static_cast<int>(reg.getWidth())) {
        throw std::invalid_argument("Invalid id for OneQBitOp");
    }

    const int OperationDimension = 2;
    int neighbours[OperationDimension];
    for (int i = 0; i < OperationDimension; i++) {
        neighbours[i] = set_bit(
                            reg.getOffset(),
                            local_id0, i)
                        / reg.getStatesSize();
    }

    std::vector<int> RealNeighbours;
    for (int i = 0; i < OperationDimension; i++) {
        bool found = std::find(RealNeighbours.begin(), RealNeighbours.end(), neighbours[i]) != RealNeighbours.end();
        if (!found) {
            RealNeighbours.push_back(neighbours[i]);
        }
    }
    std::sort(RealNeighbours.begin(), RealNeighbours.end());

    std::vector<int> temp;
    std::vector<int>::iterator inext,  it = std::find(RealNeighbours.begin(), RealNeighbours.end(), ParallelSubSystemHelper::getConfig().rank);
    inext = (it == RealNeighbours.end() - 1) ? RealNeighbours.begin() : it + 1;
    if (inext != RealNeighbours.begin()) {
        std::copy(inext, RealNeighbours.end(), std::back_inserter(temp));
    }
    std::copy(RealNeighbours.begin(), it, std::back_inserter(temp));
    RealNeighbours = temp;

    //#pragma omp parallel for schedule(static)

    const int e_tag = 0x76;
    int local_index = reg.getOffset();


    for (size_t currentNeighbour = 0; currentNeighbour < RealNeighbours.size() + 1; currentNeighbour++) {
        for (state i = 0; i < st_sz; i++) {
            int g_id = i + reg.getOffset();
            int currentId = get_bit(g_id, local_id0);
            mcomplex currentAmpl = 0.0f;
            for (int cid0 = 0; cid0 < 2; cid0++) {
                int s_id = set_bit(g_id, local_id0, cid0);
                if (s_id >= local_index
                        && s_id < local_index + static_cast<int>(reg.getStatesSize()) ) {
                    currentAmpl += m(currentId, cid0) * reg.getStates()[s_id - local_index];
                }
            }
            ampls[i] += currentAmpl;
        }
        if (currentNeighbour == RealNeighbours.size()) {
            continue;
        }
        int nextNeighbour = 0, prevNeighbour = 0;
        nextNeighbour = RealNeighbours[0];
        prevNeighbour = RealNeighbours[RealNeighbours.size() - 1];

        MPI_Status st;
        if (RealNeighbours[currentNeighbour] != ParallelSubSystemHelper::getConfig().rank) {

            MPI_Sendrecv_replace(&reg.getStates()[0],
                    reg.getStatesSize(),
                    MPI_DOUBLE_COMPLEX,
                    nextNeighbour,
                    e_tag,
                    prevNeighbour,
                    e_tag,
                    MPI_COMM_WORLD,
                    &st);
            int currentId = (RealNeighbours.size() - 1 - currentNeighbour +
                             RealNeighbours.size()) % RealNeighbours.size();

            local_index = RealNeighbours[currentId] * reg.getStates().size();
        }
    }
    reg.setStates(ampls);
    MPI_Barrier(MPI_COMM_WORLD);*/

    gWorld->GetGatesProvider()->ApplyQbitMatrix(m, reg, id0);
}
/* 000 - 1
 * 001 - 1
 *
 * 010 - 2
 * 011 - 2
 *
 * 100 - 3
 * 101 - 3
 *
 * 110 - 4.c
 * 111 - 4
 *
 * /
 */
void ApplyDiQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1)
{
/*
 *  MPI_Barrier(MPI_COMM_WORLD);
    QMatrix resm = m;
#ifdef USE_NOISE
    resm = resm * genNoise(2);
#endif
#ifdef LOGGING
    std::set<int> m_s; m_s.insert(id0); log_str(m_s);
#endif
    std::vector<mcomplex> ampls;
    ampls.resize(reg.getStates().size());
    memset(&ampls[0], 0, ampls.size() * sizeof(ampls[0]));
    state st_sz = reg.getStatesSize();
    int local_id0 = id0;
    int local_id1 = id1;
    if (local_id0 >= static_cast<int>(reg.getWidth())
            || local_id1 >= static_cast<int>(reg.getWidth())) {
        throw std::invalid_argument("Invalid id for DiQBitOp");
    }

    const int OperationDimension = 4;
    int neighbours[OperationDimension];


//    const int drank = 1;// 0 -- 1-2-3; 1 -- 2-3-0; 2 -- 3-0-1; 3 -- 0-1-2;
    for (int i = 0; i < OperationDimension; i++) {
        int currId0 = ((i & 0x2) > 0);
        int currId1 = ((i & 0x1) > 0);
        state currState = reg.getOffset();
        currState = set_bit(currState, local_id0, currId0);
        currState = set_bit(currState, local_id1, currId1);

        neighbours[i] = currState / reg.getStatesSize();
    }


    std::vector<int> RealNeighbours;
    for (int i = 0; i < OperationDimension; i++) {
        bool found = std::find(RealNeighbours.begin(), RealNeighbours.end(), neighbours[i]) != RealNeighbours.end();
        if (!found) {
            RealNeighbours.push_back(neighbours[i]);
        }
    }
    std::sort(RealNeighbours.begin(), RealNeighbours.end());

    std::vector<int> temp;
    std::vector<int>::iterator inext,  it = std::find(RealNeighbours.begin(), RealNeighbours.end(), ParallelSubSystemHelper::getConfig().rank);
    inext = (it == RealNeighbours.end() - 1) ? RealNeighbours.begin() : it + 1;
    if (inext != RealNeighbours.begin()) {
        std::copy(inext, RealNeighbours.end(), std::back_inserter(temp));
    }
    std::copy(RealNeighbours.begin(), it, std::back_inserter(temp));
    RealNeighbours = temp;



    //#pragma omp parallel for schedule(static)

    const int e_tag = 0x77;
    int local_index = reg.getOffset();

    for (size_t currentNeighbour = 0; currentNeighbour < RealNeighbours.size() + 1; currentNeighbour++) {
        for (state i = 0; i < st_sz; i++) {
            int g_id = i + reg.getOffset();
            int currentId0 = get_bit(g_id, local_id0);
            int currentId1 = get_bit(g_id, local_id1);
            int currentId = currentId0 * 2 + currentId1;
            mcomplex currentAmpl = 0.0f;
            for (int cid0 = 0; cid0 < 2; cid0++) {
                for (int cid1 = 0; cid1 < 2; cid1++) {
                    int s_id = set_bit(g_id, local_id0, cid0);
                    s_id = set_bit(s_id, local_id1, cid1);
                    if (s_id >= local_index
                            && s_id < local_index + static_cast<int>(reg.getStatesSize())) {
                        currentAmpl += m(currentId, cid0 * 2 + cid1) * reg.getStates()[s_id - local_index];
                    }
                }
            }
            ampls[i] += currentAmpl;
        }

        if (currentNeighbour == RealNeighbours.size()) {
            continue;
        }
        int nextNeighbour = 0, prevNeighbour = 0;
        nextNeighbour = RealNeighbours[0];
        prevNeighbour = RealNeighbours[RealNeighbours.size() - 1];

        MPI_Status st;
        if (RealNeighbours[currentNeighbour] != ParallelSubSystemHelper::getConfig().rank) {
            MPI_Sendrecv_replace(&reg.getStates()[0],
                    reg.getStatesSize(),
                    MPI_DOUBLE_COMPLEX,
                    nextNeighbour,
                    e_tag,
                    prevNeighbour,
                    e_tag,
                    MPI_COMM_WORLD,
                    &st);

            int currentId = (RealNeighbours.size() - 1 - currentNeighbour +
                             RealNeighbours.size()) % RealNeighbours.size();

            local_index = RealNeighbours[currentId] * reg.getStates().size();
        }

    }
    reg.setStates(ampls);
    MPI_Barrier(MPI_COMM_WORLD);
    */
    gWorld->GetGatesProvider()->ApplyDiQbitMatrix(m, reg, id0, id1);
}


void ApplyTriQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1, int id2)
{
/*
 *  MPI_Barrier(MPI_COMM_WORLD);
    QMatrix resm = m;
#ifdef USE_NOISE
    resm = resm * genNoise(3);
#endif
#ifdef LOGGING
    std::set<int> m_s; m_s.insert(id0); log_str(m_s);
#endif
    std::vector<mcomplex> ampls;
    ampls.resize(reg.getStates().size());
    memset(&ampls[0], 0, ampls.size() * sizeof(ampls[0]));
    state st_sz = reg.getStatesSize();
    int local_id0 = id0;
    int local_id1 = id1;
    int local_id2 = id2;
    if (local_id0 >= static_cast<int>(reg.getWidth())
            || local_id1 >= static_cast<int>(reg.getWidth())
            || local_id2 >= static_cast<int>(reg.getWidth())) {
        throw std::invalid_argument("Invalid id for TriQBitOp");
    }

    const int OperationDimension = 8;
    int neighbours[OperationDimension];
    for (int i = 0; i < OperationDimension; i++) {
        int currId0 = ((i & 0x4) > 0);
        int currId1 = ((i & 0x2) > 0);
        int currId2 = ((i & 0x1) > 0);

        state currState = reg.getOffset();
        currState = set_bit(currState, local_id0, currId0);
        currState = set_bit(currState, local_id1, currId1);
        currState = set_bit(currState, local_id2, currId2);

        neighbours[i] = currState / reg.getStatesSize();
    }

//    const int drank = 2;

    std::vector<int> RealNeighbours;
    for (int i = 0; i < OperationDimension; i++) {
        bool found = std::find(RealNeighbours.begin(), RealNeighbours.end(), neighbours[i]) != RealNeighbours.end();
        if (!found) {
            RealNeighbours.push_back(neighbours[i]);
        }
    }
    std::sort(RealNeighbours.begin(), RealNeighbours.end());

    std::vector<int> temp;
    std::vector<int>::iterator inext,  it = std::find(RealNeighbours.begin(), RealNeighbours.end(), ParallelSubSystemHelper::getConfig().rank);
    inext = (it == RealNeighbours.end() - 1) ? RealNeighbours.begin() : it + 1;
    if (inext != RealNeighbours.begin()) {
        std::copy(inext, RealNeighbours.end(), std::back_inserter(temp));
    }
    std::copy(RealNeighbours.begin(), it, std::back_inserter(temp));
    RealNeighbours = temp;




    //0--1-2-3  0->1->2->3->0
    //1--2-3-0
    //2--3-0-1
    //3--0-1-2


    //#pragma omp parallel for schedule(static)




    const int e_tag = 0x78;
    int local_index = reg.getOffset();
    for (size_t currentNeighbour = 0; currentNeighbour < RealNeighbours.size() + 1; currentNeighbour++) {
        for (state i = 0; i < st_sz; i++) {
            int g_id = i + reg.getOffset();

            int currentId0 = get_bit(g_id, local_id0);
            int currentId1 = get_bit(g_id, local_id1);
            int currentId2 = get_bit(g_id, local_id2);

            int currentId = currentId0 * 4 + currentId1 * 2 + currentId2;
            mcomplex currentAmpl = 0.0f;
            for (int cid0 = 0; cid0 < 2; cid0++) {
                for (int cid1 = 0; cid1 < 2; cid1++) {
                    for (int cid2 = 0; cid2 < 2; cid2++) {
                        int s_id = set_bit(g_id, local_id0, cid0);
                        s_id = set_bit(s_id, local_id1, cid1);
                        s_id = set_bit(s_id, local_id2, cid2);

                        if (s_id >= local_index
                                && s_id < local_index + static_cast<int>(reg.getStatesSize())) {
                            currentAmpl += m(currentId, cid0 * 4 + cid1 * 2 + cid2) * reg.getStates()[s_id - local_index];
                        }
                    }
                }
            }
            ampls[i] += currentAmpl;
        }

        if (currentNeighbour == RealNeighbours.size()) {
            continue;
        }
        int nextNeighbour = 0, prevNeighbour = 0;
        nextNeighbour = RealNeighbours[0];
        prevNeighbour = RealNeighbours[RealNeighbours.size() - 1];
        //        dumpVar("NEXT", drank)
        //        dumpVar(nextNeighbour, drank)
        //        dumpVar("PREV", drank)
        //        dumpVar(prevNeighbour, drank)


        MPI_Status st;
        if (RealNeighbours[currentNeighbour] != ParallelSubSystemHelper::getConfig().rank) {
            MPI_Sendrecv_replace(&reg.getStates()[0],
                    reg.getStatesSize(),
                    MPI_DOUBLE_COMPLEX,
                    nextNeighbour,
                    e_tag,
                    prevNeighbour,
                    e_tag,
                    MPI_COMM_WORLD,
                    &st);
            int currentId = (RealNeighbours.size() - 1 - currentNeighbour +
                             RealNeighbours.size()) % RealNeighbours.size();

            local_index = RealNeighbours[currentId] * reg.getStates().size();

        } else {
            std::cerr << "RANK$$$$$$$$$$$$$$$ " << ParallelSubSystemHelper::getConfig().rank << std::endl;
            std::cerr << "$$$$$$$$$$$$$$$$$$$ " << currentNeighbour << std::endl;
        }

    }
    reg.setStates(ampls);
    MPI_Barrier(MPI_COMM_WORLD);
    */
    gWorld->GetGatesProvider()->ApplyTriQbitMatrix(m, reg,  id0, id1, id2);
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

    //    std::set<int> m_s; m_s.insert(id0); m_s.insert(id1);log_str(m_s, 'S');
    //    for (int i = 0; i < reg.getStatesSize(); i++) {
    //        state st = reg.getStates()[i];
    //        int st_id0 = ((st & static_cast<state>(1 << id0)) != 0);
    //        int st_id1 = ((st & static_cast<state>(1 << id1)) != 0);
    //        st = set_bit(st, id0, st_id1);
    //        st = set_bit(st, id1, st_id0);
    //        reg.getStates()[i] = st;
    //    }

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

    //    std::set<int> m_s; m_s.insert(id0); m_s.insert(id1); m_s.insert(id2); log_str(m_s, 'C');
    //    for (int i = 0; i < reg.getStatesSize(); i++) {
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

//}


double xGenDrand()
{
    static unsigned long long x = 179425019;
    const unsigned long long p = 179425019;
    const unsigned long long q = 179425027;
    const unsigned long long m = p *q;
    const unsigned long long ULRND_MAX  =0xFFFFFFFFFFFFFFFF;
    x = (x * x) % m;
    return static_cast<double>(x) / ULRND_MAX;
}


int xGenIrand()
{
    static unsigned long long x = 179425020;
    const unsigned long long p = 179425019;
    const unsigned long long q = 179425027;
    const unsigned long long m = p *q;
//    const unsigned long long ULRND_MAX  =0xFFFFFFFFFFFFFFFF;
    x = (x * x) % m;
    return static_cast<int> (x);
}
