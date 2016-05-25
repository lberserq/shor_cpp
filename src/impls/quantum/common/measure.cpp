#include <iface/quantum/measure.h>
#include <common/quantum/common.h>
#include <common/infra/common_rand.h>
#include <cstdlib>
#include <unistd.h>

state_t Measurer::Measure(IQRegister& reg)
{
    // double rnd = static_cast<double>(rand()) / RAND_MAX;

    // double rnd = drand48();
    double rnd = xGenDrand();

//refactor it
    if (ParallelSubSystemHelper::isInited()) {
        MPI_SAFE_CALL(MPI_Bcast(&rnd, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD));
        std::vector<double> blockNorms;
        blockNorms.resize(ParallelSubSystemHelper::getConfig().size);
        double locNorm = reg.getLocalNorm();
        MPI_SAFE_CALL(MPI_Allgather(&locNorm, 1, MPI_DOUBLE, &blockNorms[0], 1, MPI_DOUBLE,
            MPI_COMM_WORLD));
        double prob = rnd;
        size_t id = 0;
        for (id = 0; id < blockNorms.size() && prob > 0; id++) {
            prob -= blockNorms[id];
        }
        if (id) {
            id--;
        }

        for (size_t i = 0; i < id; i++) {
            rnd -= blockNorms[i];
        }

        state_t res = -1;
        if (ParallelSubSystemHelper::getConfig().rank == static_cast<int>(id)) {
            for (state_t i = 0; i < reg.getStates().size(); i++) {
                mcomplex ampl = reg.getStates()[i];
                rnd -= std::abs(ampl) * std::abs(ampl);
                if (rnd < 0 || std::abs(rnd) < g_eps) {
                    res = i + reg.getOffset();
                    break;
                }
            }
        }
        //dumpVar(res, static_cast<int>(id))
        MPI_SAFE_CALL(MPI_Bcast(&res, 1, MPI_STATE_T, id, MPI_COMM_WORLD));

        return res;
    } else {
        for (uint_type i = 0; i < reg.getStatesSize(); i++) {
            if (std::abs(reg.getStates()[i]) > g_eps) {
                mcomplex ampl = reg.getStates()[i];
                long double probability = std::abs(ampl) * std::abs(ampl);
                rnd -= probability;
                if (rnd < 0 || std::abs(rnd - probability) < g_eps) {
                    return i;
                }
            }
        }
        if (std::abs(rnd) < g_eps) {
            return reg.getStatesSize() - 1;
        }
        return -1;
    }

}
