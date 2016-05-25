#include "mpi_gatesimpl.h"
#include <iostream>
#include <algorithm>
#include <iface/infra/icommonworld.h>
void MPIGatesImpl::ApplyQbitMatrix(const QMatrix &m, IQRegister &reg, int id0)
{
    ParallelSubSystemHelper::sync::barrier();
    QMatrix resm = m;
#ifdef USE_NOISE
    if (gWorld->GetNoiseProvider()->GetOperatorNoise()) {
        resm = gWorld->GetNoiseProvider()->GetOperatorNoise()->GenNoisyMatrix(m);
    }
#endif
#ifdef LOGGING
    std::set<int> m_s; m_s.insert(id0); log_str(m_s);
#endif
    std::vector<mcomplex> ampls(reg.getStates().size(), 0);
    //memset(&ampls[0], 0, ampls.size() * sizeof(ampls[0]));
    state_t st_sz = reg.getStatesSize();

    int local_id0 = id0;
    int alpha = (reg.getRepresentation() == REG_REPRESENTATION) ? 1 : 2;
    if (local_id0 >= alpha * static_cast<int>(reg.getWidth())) {
        throw std::invalid_argument("Invalid id for QBitOp");
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

    const int e_tag = 0x76;
    int local_index = reg.getOffset();


    for (size_t currentNeighbour = 0; currentNeighbour < RealNeighbours.size() + 1; currentNeighbour++) {
        for (state_t i = 0; i < st_sz; i++) {
            int g_id = i + reg.getOffset();
            int currentId = get_bit(g_id, local_id0);
            mcomplex currentAmpl = 0.0f;
            for (int cid0 = 0; cid0 < 2; cid0++) {
                int s_id = set_bit(g_id, local_id0, cid0);
                if (s_id >= local_index
                        && s_id < local_index + static_cast<int>(reg.getStatesSize()) ) {
                    currentAmpl += resm(currentId, cid0) * reg.getStates()[s_id - local_index];
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

            if ((ParallelSubSystemHelper::getConfig().size &&
                 nextNeighbour >= ParallelSubSystemHelper::getConfig().size) || nextNeighbour < 0) {

                dumpVar(ParallelSubSystemHelper::getConfig().rank, ParallelSubSystemHelper::getConfig().rank);
                dumpvec(RealNeighbours, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(nextNeighbour, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(currentNeighbour, ParallelSubSystemHelper::getConfig().rank);

                if (nextNeighbour < 0) throw "ApplyQbitMatrix:nextNeighbour<0";
                if (nextNeighbour >= 0) throw "ApplyQbitMatrix:nextNeighbour >= ParallelSubSystemHelper::getConfig().size";
            }
            if ((ParallelSubSystemHelper::getConfig().size &&
                    prevNeighbour >= ParallelSubSystemHelper::getConfig().size) || prevNeighbour < 0) {

                dumpVar(ParallelSubSystemHelper::getConfig().rank, ParallelSubSystemHelper::getConfig().rank);
                dumpvec(RealNeighbours, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(nextNeighbour, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(currentNeighbour, ParallelSubSystemHelper::getConfig().rank);

                if (prevNeighbour < 0) throw "ApplyQbitMatrix:prevNeighbour<0";
                if (prevNeighbour >= 0) throw "ApplyQbitMatrix:prevNeighbour >= ParallelSubSystemHelper::getConfig().size";
            }


            MPI_SAFE_CALL(MPI_Sendrecv_replace(&reg.getStates()[0],
                    reg.getStatesSize(),
                    MPI_DOUBLE_COMPLEX,
                    nextNeighbour,
                    e_tag,
                    prevNeighbour,
                    e_tag,
                    MPI_COMM_WORLD,
                    &st));
            int currentId = (RealNeighbours.size() - 1 - currentNeighbour +
                             RealNeighbours.size()) % RealNeighbours.size();

            local_index = RealNeighbours[currentId] * reg.getStates().size();
        }
    }
    reg.setStates(ampls);
    ParallelSubSystemHelper::sync::barrier();
}

void MPIGatesImpl::ApplyDiQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1)
{
    ParallelSubSystemHelper::sync::barrier();
    QMatrix resm = m;
#ifdef USE_NOISE
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
    int local_id1 = id1;
    int alpha = (reg.getRepresentation() == REG_REPRESENTATION) ? 1 : 2;
    if (local_id0 >= alpha * static_cast<int>(reg.getWidth())
            || local_id1 >= alpha * static_cast<int>(reg.getWidth())) {
        throw std::invalid_argument("Invalid id for DiQBitOp");
    }
    const int OperationDimension = 4;
    int neighbours[OperationDimension];


//    const int drank = 1;// 0 -- 1-2-3; 1 -- 2-3-0; 2 -- 3-0-1; 3 -- 0-1-2;
    for (int i = 0; i < OperationDimension; i++) {
        int currId0 = ((i & 0x2) > 0);
        int currId1 = ((i & 0x1) > 0);
        state_t currState = reg.getOffset();
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
        for (state_t i = 0; i < st_sz; i++) {
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
                        currentAmpl += resm(currentId, cid0 * 2 + cid1) * reg.getStates()[s_id - local_index];
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
            if ((ParallelSubSystemHelper::getConfig().size &&
                 nextNeighbour >= ParallelSubSystemHelper::getConfig().size) || nextNeighbour < 0) {

                dumpVar(ParallelSubSystemHelper::getConfig().rank, ParallelSubSystemHelper::getConfig().rank);
                dumpvec(RealNeighbours, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(nextNeighbour, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(currentNeighbour, ParallelSubSystemHelper::getConfig().rank);

                if (nextNeighbour < 0) throw "ApplyDiQbitMatrix:nextNeighbour<0";
                if (nextNeighbour >= 0) throw "ApplyDiQbitMatrix:nextNeighbour >= ParallelSubSystemHelper::getConfig().size";
            }
            if ((ParallelSubSystemHelper::getConfig().size &&
                    prevNeighbour >= ParallelSubSystemHelper::getConfig().size) || prevNeighbour < 0) {

                dumpVar(ParallelSubSystemHelper::getConfig().rank, ParallelSubSystemHelper::getConfig().rank);
                dumpvec(RealNeighbours, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(nextNeighbour, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(currentNeighbour, ParallelSubSystemHelper::getConfig().rank);

                if (prevNeighbour < 0) throw "ApplyDiQbitMatrix:prevNeighbour<0";
                if (prevNeighbour >= 0) throw "ApplyDiQbitMatrix:prevNeighbour >= ParallelSubSystemHelper::getConfig().size";
            }


            MPI_SAFE_CALL(MPI_Sendrecv_replace(&reg.getStates()[0],
                    reg.getStatesSize(),
                    MPI_DOUBLE_COMPLEX,
                    nextNeighbour,
                    e_tag,
                    prevNeighbour,
                    e_tag,
                    MPI_COMM_WORLD,
                    &st));

            int currentId = (RealNeighbours.size() - 1 - currentNeighbour +
                             RealNeighbours.size()) % RealNeighbours.size();

            local_index = RealNeighbours[currentId] * reg.getStates().size();
        }

    }
    reg.setStates(ampls);
    ParallelSubSystemHelper::sync::barrier();
}

void MPIGatesImpl::ApplyTriQbitMatrix(const QMatrix &m, IQRegister &reg, int id0, int id1, int id2)
{

    ParallelSubSystemHelper::sync::barrier();
    QMatrix resm = m;
#ifdef USE_NOISE
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
    int local_id1 = id1;
    int local_id2 = id2;

    int alpha = (reg.getRepresentation() == REG_REPRESENTATION) ? 1 : 2;
    if (local_id0 >= alpha * static_cast<int>(reg.getWidth())
            || local_id1 >= alpha * static_cast<int>(reg.getWidth())
            || local_id2 >= alpha * static_cast<int>(reg.getWidth())) {
        throw std::invalid_argument("Invalid id for TriQBitOp");
    }

    const int OperationDimension = 8;
    int neighbours[OperationDimension];
    for (int i = 0; i < OperationDimension; i++) {
        int currId0 = ((i & 0x4) > 0);
        int currId1 = ((i & 0x2) > 0);
        int currId2 = ((i & 0x1) > 0);

        state_t currState = reg.getOffset();
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

    const int e_tag = 0x78;
    int local_index = reg.getOffset();
    for (size_t currentNeighbour = 0; currentNeighbour < RealNeighbours.size() + 1; currentNeighbour++) {
        for (state_t i = 0; i < st_sz; i++) {
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
                            currentAmpl += resm(currentId, cid0 * 4 + cid1 * 2 + cid2) * reg.getStates()[s_id - local_index];
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

        MPI_Status st;
        if (RealNeighbours[currentNeighbour] != ParallelSubSystemHelper::getConfig().rank)
        {
            if ((ParallelSubSystemHelper::getConfig().size &&
                 nextNeighbour >= ParallelSubSystemHelper::getConfig().size) || nextNeighbour < 0) {

                dumpVar(ParallelSubSystemHelper::getConfig().rank, ParallelSubSystemHelper::getConfig().rank);
                dumpvec(RealNeighbours, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(nextNeighbour, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(currentNeighbour, ParallelSubSystemHelper::getConfig().rank);

                if (nextNeighbour < 0) throw "ApplyTriQbitMatrix:nextNeighbour<0";
                if (nextNeighbour >= 0) throw "ApplyTriQbitMatrix:nextNeighbour >= ParallelSubSystemHelper::getConfig().size";
            }
            if ((ParallelSubSystemHelper::getConfig().size &&
                    prevNeighbour >= ParallelSubSystemHelper::getConfig().size) || prevNeighbour < 0) {

                dumpVar(ParallelSubSystemHelper::getConfig().rank, ParallelSubSystemHelper::getConfig().rank);
                dumpvec(RealNeighbours, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(nextNeighbour, ParallelSubSystemHelper::getConfig().rank);
                dumpVar(currentNeighbour, ParallelSubSystemHelper::getConfig().rank);

                if (prevNeighbour < 0) throw "ApplyTriQbitMatrix:prevNeighbour<0";
                if (prevNeighbour >= 0) throw "ApplyTriQbitMatrix:prevNeighbour >= ParallelSubSystemHelper::getConfig().size";
            }

            MPI_SAFE_CALL(MPI_Sendrecv_replace(&reg.getStates()[0],
                    reg.getStatesSize(),
                    MPI_DOUBLE_COMPLEX,
                    nextNeighbour,
                    e_tag,
                    prevNeighbour,
                    e_tag,
                    MPI_COMM_WORLD,
                    &st));
            size_t currentId = (RealNeighbours.size() - 1 - currentNeighbour +
                             RealNeighbours.size()) % RealNeighbours.size();

            local_index = RealNeighbours[currentId] * reg.getStates().size();

        }
    }
    reg.setStates(ampls);
    ParallelSubSystemHelper::sync::barrier();
}