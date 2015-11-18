#include "tests.h"
#include "adder.h"
#include "multiplier.h"
#include "qft.h"
//#include "complex.h"
#include "measure.h"
#include <complex.h>
#include <ccomplex>
extern "C" {
    #include <quantum.h>
}
#include <cstdio>

extern "C"
{
void add_mod_n(int N,int a,int width, quantum_reg *reg);
void quantum_copy_qureg(quantum_reg *src, quantum_reg *dst);
void mul_mod_n(int N, int a, int ctl, int width, quantum_reg *reg);
void quantum_swaptheleads(int width, quantum_reg *reg);
}
#define quantum_real(z) (creal(z))
#define quantum_imag(z) (cimag(z))



int tests::width = 4;
int compare(IQRegister *x, quantum_reg *t)
{
    if (t->width == x->getWidth()) {
        std::vector<mcomplex> xst = x->getStates();
        for (int i = 0; i < t->size; i++) {
            unsigned p = t->state[i];
            bool found =  (p < xst.size() && std::abs(xst[p]) > g_eps);
            if (!found) {
                std::fprintf(stderr, "ERROR cant find state %d\n", p);
                std::fprintf(stderr, "EXPECTED\n");
                quantum_print_qureg(*t);
                std::fprintf(stderr, "FOUND\n");
                x->print();
                return 1;
            }
        }
    } else {
        if (t->width != x->getWidth()) {
            std::fprintf(stderr, "ERROR WIDTHS ARE NOT EQUAL %d vs %d\n", x->getWidth(), t->width);
        }
        return 1;
    }
    //fprintf(stderr, "OK\n");
    return 0;
}


void apply_walsh(IQRegister &reg)
{
    for (int i = 0; i <reg.getWidth(); i++) {
        ApplyHadamard(reg, i);
    }
}


void tests::AdderTest()
{
    fprintf(stderr, "ADDER TEST\n");
    const int N = 100;
    width = 4;
    //const int width = 4;
    IQRegister *p = new StaticQRegister(2 * width + 2, 2);
    quantum_reg t = quantum_new_qureg(2, 2 * width + 2);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    if (compare(p, &t)) {
        return;
    }
    for (int i = 0; i < (1 << width) + 1; i++) {
        StaticQRegister tmpp = * dynamic_cast<StaticQRegister *>(p);
        quantum_reg tmpt;
        quantum_copy_qureg(&t, &tmpt);
        Adder *adder = new Adder(i, N, width, tmpp);
        adder->perform();
        add_mod_n(N, i, width, &tmpt);
        if (compare(p, &t)) {
            fprintf(stderr, "%d\n", i);
            return;
        }

        fprintf(stderr, "%d\n", i);
        delete adder;
    }

    delete p;
    quantum_delete_qureg(&t);
    width = 12;
    IQRegister *pp = new StaticQRegister(width, 332);
    quantum_reg tt = quantum_new_qureg(332, width);
    Adder *addt = new Adder(6, 8, 2, *pp);
    add_mod_n(8, 6, 2, &tt);
    addt->perform();
    if (compare(pp, &tt)) {
        return;
    }

    fprintf(stderr, "TEST PASSED\n");
}



void tests::HadmardTest()
{
    fprintf(stderr, "Hadamard TEST\n");
    int pin = 3;
    IQRegister *p = new StaticQRegister(tests::width, 0);
    quantum_reg t = quantum_new_qureg(0, tests::width);
    ApplyHadamard(*p, pin);
    quantum_hadamard(pin, &t);
    if (compare(p, &t)) {
        delete p;
        quantum_delete_qureg(&t);
        return;
    }
    fprintf(stderr, "TEST PASSED\n");
    delete p;
    quantum_delete_qureg(&t);

}



void tests::CNOTTest() {

    fprintf(stderr, "CNOT TEST\n");
    const int N = 100;
    //const int width = 4;
    IQRegister *p = new StaticQRegister(width, 2);
    quantum_reg t = quantum_new_qureg(2, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            if (i != j) {
                ApplyCnot(*p, i, j);
                quantum_cnot(i, j, &t);
                if (compare(p, &t)) {
                    fprintf(stderr, "%d %d\n", i, j);
                    return;
                }
            }
        }
    }

    delete p;
    quantum_delete_qureg(&t);
    fprintf(stderr, "TEST PASSED\n");

}

void tests::ToffoliTest() {

    fprintf(stderr, "TOFFOLITEST\n");
    //const int N = 100;
    //const int width = 4;
    IQRegister *p = new StaticQRegister(width, 2);
    quantum_reg t = quantum_new_qureg(2, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < width; k++) {
                if (i != j && j != k && k != i) {
                    ApplyToffoli(*p, i, j, k);
                    quantum_toffoli(i, j, k, &t);
                    if (compare(p, &t)) {
                        fprintf(stderr, "%d %d %d \n", i, j, k);
                        return;
                    }
                }
            }
        }
    }
    IQRegister *tmp1 = new StaticQRegister(12, 7);
    quantum_reg tmp2 = quantum_new_qureg(7, 12);
    quantum_toffoli(2, 1, 0, &tmp2);
    ApplyToffoli(*tmp1, 2, 1, 0);
    if (compare(tmp1, &tmp2)) {
        return;
    }

    delete p;
    quantum_delete_qureg(&t);

    fprintf(stderr, "TEST PASSED\n");

}



void tests::NotTest() {
    fprintf(stderr, "NOTTEST\n");
    //const int width = 4;
    IQRegister *p = new StaticQRegister(width, 2);
    quantum_reg t = quantum_new_qureg(2, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    for (int i = 0; i < width; i++) {
        ApplyNot(*p, i);
        quantum_sigma_x(i, &t);
        if (compare(p, &t)) {
            fprintf(stderr, "%d\n", i);
            return;
        }
    }

    delete p;
    quantum_delete_qureg(&t);
    fprintf(stderr, "TEST PASSED\n");
}

void tests::SwapTest()
{
    fprintf(stderr, "SWAP_TEST\n");
    width =  4;
    IQRegister *p = new StaticQRegister(2 * width, 0);
    quantum_reg t = quantum_new_qureg(0, width * 2);
    apply_walsh(*p);

    quantum_walsh(width, &t);
    QFT::ApplyQFT(*p, width);
    quantum_qft(width, &t);
    quantum_swaptheleads(width, &t);
    RegSwapLR(width, *p);
    if (compare(p, &t)) {
        return;
    }

    fprintf(stderr, "TEST_PASSED\n");
}




void exp_test_fun(IQRegister *p, quantum_reg *t, int a, int N, int swidth, int width)
{
    ApplyNot(*p, 2 * swidth + 2);
    quantum_sigma_x(2 * swidth + 2, t);
    if (compare(p, t)) {
        return;
    }
    for (int i = 1; i <= width; i++) {
        int pow = a % N;
        for (int j = 1; j < i; j++) {
            pow *= pow;
            pow %= N;
        }
        //Multiplier(IQRegister &in, int N, int width, int a, int ctl_id);
        Multiplier *mp = new Multiplier(*p, N, swidth, pow, 3  * swidth + 1 + i);
        *p = mp->perform();
        //mul_mod_n(N,f,3*width+1+i, width, reg);
        mul_mod_n(N, pow, 3 * swidth + 1 + i, swidth, t);
        if (compare(p, t)) {
            return;
        }
    }
}


void tests::MulTest()
{
    fprintf(stderr, "MULTIPLIER TEST\n");
    const int N = 8;
    //const int width = 4;
    width = 4;
    int swidth = 2;
    const int ctl_id = 3 * swidth + 2;
    IQRegister *p = new StaticQRegister(width, 0);
    quantum_reg t = quantum_new_qureg(0, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    if (compare(p, &t)) {
        return;
    }
    p->allocSharedMem(ctl_id);
    quantum_addscratch(ctl_id, &t);
    if (compare(p, &t)) {
        return;
    }

    ApplyNot(*p, 2 * swidth + 2);
    quantum_sigma_x(2 * swidth + 2, &t);

    if (compare(p, &t)) {
        return;
    }


    int a = 7;
    Multiplier *mp = new Multiplier(*p, N, swidth, a, ctl_id);
    *p = mp->perform();
    mul_mod_n(N, a, ctl_id, swidth, &t);
    if (compare(p, &t)) {
        return;
    }

    /*for (int i = 1; i < N; i++) {
        IQRegister tmpp = *p;
        quantum_reg tmpt;
        quantum_copy_qureg(&t, &tmpt);
        Multiplier *op = new Multiplier(tmpp, N, width, i, ctl_id);
        op->perform();
        mul_mod_n(N, i, ctl_id, width, &tmpt);
        if (compare(p, &t)) {
            fprintf(stderr, "%d\n", i);
            return;
        }
        delete op;

        fprintf(stderr, "%d\n", i);
    }*/



    delete p;
    quantum_delete_qureg(&t);
    fprintf(stderr, "TEST PASSED\n");
}


void tests::expTest() {
    // not implemented yet
    fprintf(stderr, "EXPAMODN test\n");
    width =  4;
    int swidth = 2;
    IQRegister *p = new StaticQRegister(width, 0);
    quantum_reg t = quantum_new_qureg(0, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    if (compare(p, &t)) {
        return;
    }
    quantum_addscratch(3*swidth+2, &t);
    p->allocSharedMem(3 * swidth + 2);

    if (compare(p, &t)) {
        return;
    }
    const int a = 7;
    const int N = 8;
    expamodn(*p, N, a, width, swidth);
    quantum_exp_mod_n(N, a, width, swidth, &t);
    //exp_test_fun(p, &t, a, N, swidth, width);


    if (compare(p, &t)) {
        return;
    }
    //fprintf(stderr, "TEST PASSED\n");
    for(int i=0;i<3*swidth+2;i++)
    {
        quantum_bmeasure(0, &t);
    }
    DeleteLocalVars(*p, 3 * swidth + 2);

    quantum_qft(width, &t);
    QFT::ApplyQFT(*p, width);
    if (compare(p, &t)) {
        return;
    }
    int c = quantum_measure(t);
    int q = Measurer::Measure(*p);
    if (c != q) {
        fprintf(stderr, "BAYDA\n");
        return;
    }

    fprintf(stderr, "PASSED\n");

}

void tests::collapseTest() {
    fprintf(stderr, "COLLAPSE_TEST\n");
    IQRegister *p = new StaticQRegister(width, 0);
    quantum_reg t = quantum_new_qureg(0, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    int swidth = 6;
    quantum_addscratch(swidth, &t);
    p->allocSharedMem(swidth);

    quantum_cnot(0, 1, &t);
    ApplyCnot(*p,0, 1);
    for (int i = 0; i < swidth; i++) {
        quantum_bmeasure(0, &t);
    }

    DeleteLocalVars(*p, swidth);

    if (compare(p, &t)) {
        return;
    }
    fprintf(stderr, "PASSED\n");
}


/*void tests::expTest() {
    // not implemented yet
    fprintf(stderr, "EXPAMODN test\n");
    width =  4;
    int swidth = 2;
    IQRegister *p = new StaticQRegister(width, 0);
    quantum_reg t = quantum_new_qureg(0, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    if (compare(p, &t)) {
        return;
    }
    quantum_addscratch(3*swidth+2, &t);
    p->alloc_smem(3 * swidth + 2);

    if (compare(p, &t)) {
        return;
    }
    const int a = 7;
    const int N = 8;
    expamodn(*p, N, a, width, swidth);
    quantum_exp_mod_n(N, a, width, swidth, &t);
    if (compare(p, &t)) {
        return;
    }
    fprintf(stderr, "TEST PASSED\n");
}*/


void tests::QFTTest() {
    fprintf(stderr, "QFTTEST\n");
    //const int width = 4;
    width = 3;
    IQRegister *p = new StaticQRegister(width, 2);
    ApplyToffoli(*p, 0, 1, 2);
    log_print();
    quantum_reg t = quantum_new_qureg(2, width);
    apply_walsh(*p);
    log_print();
    quantum_walsh(width, &t);
    if (compare(p, &t)) {
        return;
    }
    QFT::ApplyQFT(*p, width);
    quantum_qft(width, &t);
    if (compare(p, &t)) {
        return;
    }
    delete p;
    quantum_delete_qureg(&t);
    fprintf(stderr, "TEST PASSED\n");
}


void tests::MeasureTest() {
    fprintf(stderr, "MTEST\n");
    //const int width = 4;
    width = 5;
    IQRegister *p = new StaticQRegister(width, 2);
    quantum_reg t = quantum_new_qureg(2, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    if (compare(p, &t)) {
        return;
    }
    QFT::ApplyQFT(*p, width);
    quantum_qft(width, &t);
    if (compare(p, &t)) {
        return;
    }
    state q = Measurer::Measure(*p);
    if (q != (state)(-1)) {
        fprintf(stderr, "TEST PASSED\n");
    }
}


