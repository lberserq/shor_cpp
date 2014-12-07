#ifndef QREGISTER_H
#define QREGISTER_H
#include <vector>
#include "config.h"
/*! \class Qvector
 *\brief Register of Qubits
 *States with amplitudes stored in states
 *Amplitudes of this states stored
 *
 */
class Qregister
{
    std::vector<mcomplex> ampl;
    std::vector<state> states;
    int m_width;
public:
    /*!
     *\fn Creates the new Qregister with size width and one initial state
     */
    Qregister(int width, state init_state);

    /*!
     *\fn Returns width of Qregister
     */

    int getWidth() const {
        return m_width;
    }

    /*!
     *\fn Returns states of register
     */

    std::vector<state> &getStates() {
        return states;
    }

    /*!
     *\fn Returns amplitudes of Register
     */

    std::vector<mcomplex> &getAmpls() {
        return ampl;
    }

    const int getSize() const {
        return states.size();
    }

    /*!
     *\fn Setter for amplitudes
     */
    void setAmpls(const std::vector<mcomplex> &amp) {
        ampl = amp;
    }

    /*!
     *\fn Setter for states
     */
    void setStates(const std::vector<state> &st) {
        states = st;
    }


    /*!
     *\fn Alloc additional size(increase width of Qregister)
     */
    void alloc_smem(int size);

    /*!
     *\fn Setter for amplitudes
     */
    /*!
     *\fn Remove states with \param
     *
     */

    void collapse_state(int id, long double prob_amp);




    friend void reg_swap(int width, Qregister &in);
    void print();
};

void DeleteLocalVars(Qregister &reg, int size);
void SwapXY(Qregister &reg, int width_x);

#endif // QREGISTER_H
