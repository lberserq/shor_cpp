#ifndef IQRegister_H
#define IQRegister_H
#include <vector>
#include "config.h"

//class IQRegHelper {
//    void
//};


class IQRegister {
    friend class IQRegHelper;
    //EKA_DECLARE_IID(IIQRegister)
public:
    virtual result_t FinalConstruct(int width, state StartState);
    virtual size_t getWidth() const = 0;
    virtual std::vector<mcomplex> & getStates() = 0;
    virtual void setStates(const std::vector<mcomplex> &v) = 0;
    virtual size_t getStatesSize() const  = 0;
    virtual void allocSharedMem(int width) = 0;
    virtual void collapseState(int id, long double amplProb) = 0;
    virtual void print() = 0;
    virtual void printNorm() = 0;
    virtual ~IQRegister();
};

class StaticQRegister : public IQRegister {
    std::vector<mcomplex> m_states;
    size_t m_width;
    size_t m_alloc_width;
public:
    StaticQRegister(int width, state startState, const mcomplex & amplitude = 1.0);

    size_t getWidth() const;
    std::vector<mcomplex> & getStates();
    void setStates(const std::vector<mcomplex> &v);
    size_t getStatesSize() const;
    void allocSharedMem(int width);
    void collapseState(int id, long double amplProb);
    void print();
    void printNorm();
};





//class IQRegister : public IIQRegister
//{
//    std::vector<mcomplex> ampl;
//    std::vector<state> states;
//    int m_width;
//public:
//    void printnorm();
//    /*!
//     *\fn Creates the new IQRegister with size width and one initial state
//     */
//    IQRegister(int width, state init_state);

//    /*!
//     *\fn Returns width of IQRegister
//     */

//    int getWidth() const {
//        return m_width;
//    }

//    /*!
//     *\fn Returns states of register
//     */

//    std::vector<state> &getStates() {
//        return states;
//    }

//    /*!
//     *\fn Returns amplitudes of Register
//     */

//    std::vector<mcomplex> &getStates() {
//        return ampl;
//    }

//    state getSize() const {
//        return ampl.size();
//    }

//    /*!
//     *\fn Setter for amplitudes
//     */
//    void setAmpls(const std::vector<mcomplex> &amp) {
//        ampl = std::move(amp);
//    }

//    /*!
//     *\fn Setter for states
//     */
//    void setStates(const std::vector<state> &st) {
//        states = st;
//    }


//    /*!
//     *\fn Alloc additional size(increase width of IQRegister)
//     */
//    void alloc_smem(int size);

//    /*!
//     *\fn Setter for amplitudes
//     */

//    void collapse_state(int id, long double prob_amp);

//    /*!
//     * \brief shrink_reg
//     * \param new_width new width of register
//     */

//    void shrink_reg(int new_width);


//    /*!
//     * \brief reg_swap swap parts the register
//     * \param width size of swapped parts
//     * \param in   reg
//     */




//    friend void reg_swap(int width, IQRegister &in);
//    void print();
//};



void RegSwapLR(int width, IQRegister &in);
void DeleteLocalVars(IQRegister &reg, int size);
void SwapXY(IQRegister &reg, int width);

#endif // IQRegister_H
