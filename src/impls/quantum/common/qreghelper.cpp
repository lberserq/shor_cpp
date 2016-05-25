#include <iface/quantum/iqregister.h>
#include <common/quantum_common_lib.h>
namespace QRegHelper {

    void DeleteVar(IQRegister &reg, int id)
    {
        long double probs = 0.0f;
        state_t m_sz = reg.getStates().size();
        for (state_t i = 0; i < m_sz; i++) {
            if (std::abs(reg.getStates()[i]) > g_eps) {
                state_t st = i + reg.getOffset();
                int state_id = ((st & static_cast<state_t>(1 << id)) != 0);
                if (!state_id) {
                    mcomplex amp = reg.getStates()[i];
                    probs += std::pow(std::abs(amp), 2);
                }
            }
        }
        if (ParallelSubSystemHelper::isInited()) {

            long double gprobs = 0.0;
            MPI_SAFE_CALL(MPI_Allreduce(&probs, &gprobs, 1, MPI_LONG_DOUBLE, MPI_SUM,
                          MPI_COMM_WORLD));
            probs = gprobs;
        }



        //long double rnd_num =  static_cast<long double>(rand()) / RAND_MAX;
        long double rnd_num = xGenDrand();
        if (ParallelSubSystemHelper::isInited()) {
            MPI_SAFE_CALL(MPI_Bcast(&rnd_num, 1, MPI_LONG_DOUBLE, 0, MPI_COMM_WORLD));
        }
        long double targ_val = 0.0f;
        if (rnd_num> probs) {
            targ_val = 1.0f;
        }
        reg.collapseState(id, targ_val);
    }

    void DeleteLocalVars(IQRegister &reg, int size)
    {
        //int drank = 0;
        for (int i = 0; i < size; i++) {
            DeleteVar(reg, 0);
            if (reg.getRepresentation() != REG_REPRESENTATION) {
                DeleteVar(reg, reg.getWidth());
            }
            if (!ParallelSubSystemHelper::isInited())
            {
                real_t norm = reg.getLocalNorm();
                if (std::abs(norm - 1) > 1e-4) {
                    throw "DeleteVar failed";
                }
            }
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
        if (width > static_cast<int>(in.getWidth() / 2)) {
            throw std::invalid_argument("Invalid width for RegSwapLR");
        }
        if (!ParallelSubSystemHelper::isInited() || ParallelSubSystemHelper::getConfig().size == 1) {
                    std::vector<state_t> news;
                    std::vector<mcomplex> ampls;
                    size_t m_sz = in.getStatesSize();
                    for (state_t i  = 0; i < m_sz; i++) {
                        if (std::abs(in.getStates()[i]) > g_eps) {
                            state_t left = i % static_cast<state_t>(1 << width);
                            state_t right = 0;
                            for (int j = 0; j < width; j++) {
                                right |= i & static_cast<state_t>(1 << (width + j));
                            }
                            state_t x = i ^ (left | right);
                            x |= static_cast<state_t>(left << width);
                            x |= (right >> width);
                            news.push_back(x);
                            ampls.push_back(in.getStates()[i]);
                        }
                    }

                    in.getStates().clear();
                    in.getStates().resize(m_sz);
                    m_sz = news.size();

                    for (state_t i = 0; i < m_sz; i++) {
                        in.getStates()[news[i]] = ampls[i];
                    }
        } else {
            for (int i = 0; i < width; i++) {
                ApplySWAP(in, i, i + width);
            }
        }

    }
}
