#include "tests.h"

#include <common/quantum_common_impl.h>
#include <common/quantum_common_lib.h>
#include <complex.h>
#include <common/quantum/qreghelper.h>
#include "../src/impls/infra/serializers/serializers_all.h"
#if (__cplusplus >= 201103L)
#include <ccomplex>
#endif
extern "C" {
#include <quantum.h>
}
#include <cstdio>

typedef SharedSimpleQRegister UserDefQRegister;
extern "C"
{
void add_mod_n(int N,int a,int width, quantum_reg *reg);
void quantum_copy_qureg(quantum_reg *src, quantum_reg *dst);
void mul_mod_n(int N, int a, int ctl, int width, quantum_reg *reg);
void quantum_swaptheleads(int width, quantum_reg *reg);
}
#define quantum_real(z) (creal(z))
#define quantum_imag(z) (cimag(z))

#define ParallelLogger(Stream, msg) { if (!ParallelSubSystemHelper::getConfig().rank) {fprintf(stderr, "%s\n", msg);}}

int tests::width = 3;
int compare(IQRegister *x, quantum_reg *t)
{
    const double _t_eps = 1e-5;
    if (t->width == static_cast<int>(x->getWidth()) ) {
        std::vector<mcomplex> xst = x->getAllReg();
        unsigned char errorFlag = 0;
        ParallelSubSystemHelper::sync::barrier();
        if (!ParallelSubSystemHelper::getConfig().rank) {
            for (int i = 0; i < t->size; i++) {
                unsigned p = t->state[i];
                std::complex<float> pp = t->amplitude[i];
                std::complex<double> dpp = pp;
                bool found =  (p < xst.size() && std::abs(xst[p] - dpp) < _t_eps);
                if (found) {
                //    fprintf(stderr, "Found state %d p with %lf ampl\n", p, std::abs(dpp));
                }
                if (!found) {
                    std::fprintf(stderr, "ERROR cant find state %d\n Expected:\n", p);
                    std::fflush(stderr);
                    quantum_print_qureg(*t);
                    std::fflush(stdout);
                    std::fprintf(stderr, "FOUND\n");
                    //x->print();
                    errorFlag = 1;
                    break;
                    //return 1;
                }
            }
        }

        ParallelSubSystemHelper::sync::barrier();
        byte_t gErrorFlag = 0;
        MPI_Allreduce(&errorFlag,
                      &gErrorFlag,
                      1,
                      MPI_BYTE,
                      MPI_LOR,
                      MPI_COMM_WORLD);
        if (gErrorFlag) {

            x->print();
            ParallelSubSystemHelper::sync::barrier();
            MPI_Abort(MPI_COMM_WORLD, -1);
            return 1;
        }

    } else {
        if (t->width != static_cast<int>(x->getWidth()) ) {
            if (!ParallelSubSystemHelper::getConfig().rank){
                std::fprintf(stderr, "ERROR WIDTHS ARE NOT EQUAL %ld vs %d\n", x->getWidth(), t->width);
            }
            MPI_Abort(MPI_COMM_WORLD, -1);
            return 1;
        }
    }
    //fprintf(stderr, "OK\n");
    return 0;
}


void apply_walsh(IQRegister &reg)
{
    for (int i = 0; i < static_cast<int>(reg.getWidth()) ; i++) {
        ApplyHadamard(reg, i);
    }
}


void tests::AdderTest()
{
    ParallelLogger(stderr, "ADDER TEST\n");
    const int N = 100;
    width = 3;
    //const int width = 4;
    IQRegister *p = new UserDefQRegister(2 * width + 2, 2);
    quantum_reg t = quantum_new_qureg(2, 2 * width + 2);
    apply_walsh(*p);
    quantum_walsh(2 * width + 2, &t);
    if (compare(p, &t)) {
        return;
    }
    for (int i = 0; i < (1 << width) + 1; i++) {
        UserDefQRegister tmpp = * dynamic_cast<UserDefQRegister *>(p);
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
    int old_width = width;
    width = 12;
    IQRegister *pp = new UserDefQRegister(width, 332);
    quantum_reg tt = quantum_new_qureg(332, width);
    Adder *addt = new Adder(6, 8, 2, *pp);
    add_mod_n(8, 6, 2, &tt);
    addt->perform();
    if (compare(pp, &tt)) {
        return;
    }
    width = old_width;

    ParallelLogger(stderr, "TEST PASSED\n");
}
/* Ids Ranks    Neighbours
 * 0000 - 0  -1, 1
 * 0001 - 0
 * 0010 - 0
 * 0011 - 0
 * 0100 - 1  0, -1
 * 0101 - 1
 * 0110 - 1
 * 0111 - 1
 * 1000 - 2 -1, 3
 * 1001 - 2
 * 1010 - 2
 * 1011 - 2
 * 1100 - 3 2, -1
 * 1101 - 3
 * 1110 - 3
 * 1111 - 3
 *
 *
 *
 *
 *
 *
 */


void tests::HadmardTest()
{
    ParallelSubSystemHelper::sync::barrier();
    q_log("HTEST");
    ParallelLogger(stderr, "Hadamard TEST\n");
    int pin = tests::width - 1;
    IQRegister *p = new UserDefQRegister(tests::width, 0);
    quantum_reg t = quantum_new_qureg(0, tests::width);
    ApplyHadamard(*p, pin);
    quantum_hadamard(pin, &t);
    if  (compare(p, &t)) {
        return;
    }
    for (int i = 0; i < (1 << tests::width); i++) {
        for (int j = 0; j < tests::width; j++ ) {
            IQRegister *pp = new UserDefQRegister(tests::width, i);
            quantum_reg tt = quantum_new_qureg(i, tests::width);
            ApplyHadamard(*pp, j);
            quantum_hadamard(j, &tt);
            if  (compare(pp, &tt)) {
                return;
            }
        }
    }

    if (compare(p, &t)) {
        delete p;
        quantum_delete_qureg(&t);
        return;
    }
    ParallelLogger(stderr, "TEST PASSED\n");
    delete p;
    quantum_delete_qureg(&t);

}



void tests::CNOTTest() {

    ParallelSubSystemHelper::sync::barrier();
    ParallelLogger(stderr, "CNOT TEST\n");
    //const int N = 100;
    const int width = 2;
    IQRegister *p = new UserDefQRegister(width, 2);
    quantum_reg t = quantum_new_qureg(2, width);
    quantum_cnot(1, 0, &t);
    ApplyCnot(*p, 1, 0);
    if (compare(p, &t)) {
        return;
    }
    //return;

    apply_walsh(*p);
    quantum_walsh(width, &t);
    if (compare(p, &t)) {
        return;
    }
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
    ParallelLogger(stderr, "TEST PASSED\n");

}

void tests::ToffoliTest() {

    ParallelLogger(stderr, "TOFFOLITEST\n");
    //const int N = 100;
    //const int width = 4;
    const int width = 3;
    IQRegister *p = new UserDefQRegister(width, 2);
    quantum_reg t = quantum_new_qureg(2, width);
    ApplyToffoli(*p, 0, 1, 2);

    quantum_toffoli(0, 1, 2, &t);
    if (compare(p, &t)) {
        return;
    }

    apply_walsh(*p);
    quantum_walsh(width, &t);
    if (compare(p, &t)) {
        return;
    }
    ApplyToffoli(*p, 0, 1, 2);
    quantum_toffoli(0, 1, 2, &t);
    if (compare(p, &t)) {
        return;
    }

    //return;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            for (int k = 0; k < width; k++) {
                if (i != j && j != k && k != i) {
                    if (!ParallelSubSystemHelper::getConfig().rank) {
                        fprintf(stderr, "%d %d %d \n", i, j, k);
                    }
                    ApplyToffoli(*p, i, j, k);
                    quantum_toffoli(i, j, k, &t);
                    if (!ParallelSubSystemHelper::getConfig().rank) {
                        fprintf(stderr, "PASSED\n");
                    }
                    if (compare(p, &t)) {
                        fprintf(stderr, "%d %d %d \n", i, j, k);
                        return;
                    }
                }
            }
        }
    }
    IQRegister *tmp1 = new UserDefQRegister(12, 7);
    quantum_reg tmp2 = quantum_new_qureg(7, 12);
    quantum_toffoli(2, 1, 0, &tmp2);
    ApplyToffoli(*tmp1, 2, 1, 0);
    if (compare(tmp1, &tmp2)) {
        return;
    }

    delete p;
    quantum_delete_qureg(&t);

    ParallelLogger(stderr, "TEST PASSED\n");

}



void tests::NotTest() {
    ParallelLogger(stderr, "NOTTEST\n");
    //const int width = 4;
    IQRegister *p = new UserDefQRegister(width, 2);
    quantum_reg t = quantum_new_qureg(2, width);
    ApplyNot(*p, 0);
    quantum_sigma_x(0, &t);
    if (compare(p, &t)) {
        return;
    }


    //return;



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

    ParallelLogger(stderr, "TEST PASSED\n");
}

void tests::SwapTest()
{
    ParallelLogger(stderr, "SWAP_TEST\n");
    IQRegister *p = new SharedSimpleQRegister(2 * width, 1);
    quantum_reg t = quantum_new_qureg(1, width * 2);
//    p->print();
//    MPI_Barrier(MPI_COMM_WORLD);
//    ApplySWAP(*p, 0, 1);
//    p->print();
//    MPI_Barrier(MPI_COMM_WORLD);
    apply_walsh(*p);
    quantum_walsh(2 * width, &t);
    //return;
    dumpVar("FINISHED", 0)
    if (compare(p, &t)) {
        return;
    }
    dumpVar("WALSH", 0)

    QFT::ApplyQFT(*p, width);
    quantum_qft(width, &t);
    if(compare(p, &t)) {
        return;
    }
    dumpVar("QFT", 0)
    quantum_swaptheleads(width, &t);
    QRegHelper::RegSwapLR(width, *p);
    if (compare(p, &t)) {
        return;
    }

    ParallelLogger(stderr, "TEST_PASSED\n");
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

    ParallelSubSystemHelper::sync::barrier();
    ParallelLogger(stderr, "MULTIPLIER TEST\n");
    const int N = 8;
    //const int width = 4;
    width = 4;
    int swidth = 2;
    const int ctl_id = 3 * swidth + 2;
    IQRegister *p = new UserDefQRegister(width, 0);
    quantum_reg t = quantum_new_qureg(0, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);

    if (compare(p, &t)) {
        ParallelLogger(stderr, "WALSH PART FAILED\n");
        return;
    } else {

        ParallelLogger(stderr, "WALSH PART SUCCEEDED\n");
    }
//    MPI_Barrier(MPI_COMM_WORLD);
//    p->print();
//    MPI_Barrier(MPI_COMM_WORLD);

    p->allocSharedMem(ctl_id);
    quantum_addscratch(ctl_id, &t);

    if (compare(p, &t)) {
        ParallelLogger(stderr, "SHARED_PART FAILED\n");
        return;
    } else {
        ParallelLogger(stderr, "SHARED_PART SUCCEEDED\n");
    }

    ApplyNot(*p, 2 * swidth + 2);
    quantum_sigma_x(2 * swidth + 2, &t);

    if (compare(p, &t)) {
        ParallelLogger(stderr, "SIGMA FAILED\n");
        return;
    } else {
        ParallelLogger(stderr, "SIGMA SUCCEEDED\n");
    }


    int a = 7;
    Multiplier *mp = new Multiplier(*p, N, swidth, a, ctl_id);
    *p = mp->perform();
    mul_mod_n(N, a, ctl_id, swidth, &t);



    if (compare(p, &t)) {
        ParallelLogger(stderr, "SHARED_PART FAILED\n");
        return;
    } else {
        ParallelLogger(stderr, "SHARED_PART SUCCEEDED\n");
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
    ParallelLogger(stderr, "TEST PASSED\n");
}


namespace
{
    int get_dim(int_t n) {
        int_t i = 0;
        while (static_cast<int_t>(1<< i) < n) {
            i++;
        }
        return i;
    }
}

void tests::MatrixTest()
{
    ParallelLogger(stderr, "MATRIX TEST\n");
    IOperatorNoise *noise = new UnitaryNoiseImpl();
    for (int i = 1; i <= 3; ++i) {
        int sz = (1 << i);
        QMatrix m(sz, sz);
        for (int j = 0; j < sz; j++) {
            m(j, j) = 1.0;
        }

        QMatrix p = noise->GenNoisyMatrix(m);
        q_log(p.toString());
        for (int col = 0; col < sz; ++col) {
            mcomplex sum = 0;
            for (int row = 0; row < sz; ++row) {
                sum += p(row, col) * p(row, col);
            }
            if (std::abs(sum - 1.0) > g_eps) {
                q_log(i);
                q_log(sz);
                q_log(sum);
                ParallelLogger(stderr, "MATRIX TEST FAIL\n");
                return;
            }
        }
    }
    ParallelLogger(stderr, "MATRIX TEST PASSED\n");
}

void tests::CraussTest()
{
    ParallelLogger(stderr, "CRAUSS TEST\n");
    gWorld = service_ptr_t<ICommonWorld>(new CommonWorld<MPIGatesImpl>(CreateNoiseProviderStrDimImpl<NoNoiseImpl, CraussNoiseDensityImpl>("/home/lberserq/tmp/test.xml", 1 )));
    int pwidth = 2;
    IQRegister *reg = new UserDefQRegister(pwidth, 0, 1.0, MATRIX_REPRESENTATION);
    reg->print();
    ApplyHadamard(*reg, 0);
    reg->print();
    ApplyCnot(*reg, 0, pwidth - 1);
    reg->print();
    ApplyCnot(*reg, 0, 1);
    reg->print();

    ApplyToffoli(*reg, 0, 1, pwidth - 1);
    reg->print();
    reg->printNorm();


    ParallelLogger(stderr, "CRAUSS TEST PASSED\n");
}


void tests::SerializeTest()
{
    ParallelLogger(stderr, "SER_TEST\n");
    int pwidth = 2;
    service_ptr_t<IQRegister> reg(new UserDefQRegister(pwidth, 0, 1.0, MATRIX_REPRESENTATION));
    service_ptr_t<IQRegisterFactory> regFactory(new StandardQRegisterFactory<UserDefQRegister>());

    ApplyHadamard(*reg, 0);
    ApplyCnot(*reg, 0, pwidth - 1);
    ApplyCnot(*reg, 0, 1);

    ApplyToffoli(*reg, 0, 1, pwidth - 1);


    BinaryRegSerializer serializer;
   // CSerContext context(NULL, 2);

    //serializer.serialize(*reg, context);

    byteArray array1(1024 * 10);
    CSerContext context2(NULL,-1, NULL, array1);
    serializer.serialize(*reg, context2);
    IQRegister *reg2;
    X_ASSERT(X_SOK == serializer.deserialize(&reg2, context2, *regFactory.get()));
    dumpvec(context2.buffer, 0);


    ParallelLogger(stderr, "SER TEST PASSED\n");
}

void tests::ShorTest()
{

    if (ParallelSubSystemHelper::isInited()) {
        ParallelLogger(stderr, "No shor test in NUMA mode");
        return;
    }

    ParallelLogger(stderr, "SHOR TEST\n");
    const int n = 6;
    const int a = 5;
    int lwidth = get_dim(n * n);
    int swidth = get_dim(n);

    IQRegister *p = new UserDefQRegister(lwidth, 0);
    quantum_reg qr = quantum_new_qureg(0, lwidth);
    for(int i=0;i<lwidth;i++) {
        quantum_hadamard(i, &qr);
        ApplyHadamard(*p, i);
    }


    if (compare(p, &qr)) {
        ParallelLogger(stderr, "WALSH PART FAILED\n");
        return;
    } else {

        ParallelLogger(stderr, "WALSH PART SUCCEEDED\n");
    }

    quantum_addscratch(3*swidth+2, &qr);
    p->allocSharedMem(3 * swidth + 2);


    if (compare(p, &qr)) {
        ParallelLogger(stderr, "SHARED_PART FAILED\n");
        return;
    } else {
        ParallelLogger(stderr, "SHARED_PART SUCCEEDED\n");
    }


    quantum_exp_mod_n(n, a, lwidth, swidth, &qr);
    expamodn(*p, n, a, lwidth, swidth);


    if (compare(p, &qr)) {
        ParallelLogger(stderr, "EXP_PART FAILED\n");
        return;
    } else {
        ParallelLogger(stderr, "EXP_PART  SUCCEEDED\n");
    }


    for(int i=0;i<3*swidth+2;i++)
    {
        quantum_bmeasure(0, &qr);
    }
    QRegHelper::DeleteLocalVars(*p, 3 * swidth + 2);

    if (compare(p, &qr)) {
        ParallelLogger(stderr, "DELETE_PART FAILED\n");
        return;
    } else {
         ParallelLogger(stderr, "DELETE_PART SUCCEEDED\n");
    }

    quantum_qft(lwidth, &qr);
    QFT::ApplyQFT(*p, lwidth);

    if (compare(p, &qr)) {
        ParallelLogger(stderr, "QFT PART FAILED\n");
        return;
    } else {
         ParallelLogger(stderr, "QFT PART SUCCEEDED\n");
    }

    for(int i=0; i<lwidth/2; i++)
      {
        quantum_cnot(i, lwidth-i-1, &qr);
        quantum_cnot(lwidth-i-1, i, &qr);
        quantum_cnot(i, lwidth-i-1, &qr);

        ApplyCnot(*p, i, lwidth-i-1);
        ApplyCnot(*p, lwidth-i-1, i);
        ApplyCnot(*p, i, lwidth-i-1);
      }

    if (compare(p, &qr)) {
        ParallelLogger(stderr, "TEST FAILED\n");
        return;
    }

    if (compare(p, &qr)) {
        ParallelLogger(stderr, "DEINIT PART FAILED\n");
        return;
    } else {
         ParallelLogger(stderr, "DEINIT PART SUCCEEDED\n");
    }


    ParallelLogger(stderr, "TEST PASSED\n");




}



void tests::expTest() {
    // not implemented yet
    ParallelLogger(stderr, "EXPAMODN test\n");
    width =  2;
    int swidth = 1;
    IQRegister *p = new UserDefQRegister(width, 0);
    quantum_reg t = quantum_new_qureg(0, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    if (compare(p, &t)) {
        return;
    }
    quantum_addscratch(3 * swidth + 2, &t);
    p->allocSharedMem(3 * swidth + 2);

    if (compare(p, &t)) {
        return;
    }
    dumpVar("ASHM", 0)
    const int a = 2;
    const int N = 3;
    expamodn(*p, N, a, width, swidth);
    quantum_exp_mod_n(N, a, width, swidth, &t);
    //exp_test_fun(p, &t, a, N, swidth, width);

    if (compare(p, &t)) {
        return;
    }
    dumpVar("EA%N", 0)
    //fprintf(stderr, "TEST PASSED\n");
    ParallelSubSystemHelper::sync::barrier();
    //p->print();
    //quantum_print_qureg(t);
    ParallelSubSystemHelper::sync::barrier();

    for(int i=0;i<3*swidth+2;i++)
    {
        quantum_bmeasure(0, &t);
        QRegHelper::DeleteLocalVars(*p, 1);

    }
    //QRegHelper::DeleteLocalVars(*p, 3 * swidth + 2);
    if (compare(p, &t)) {
        return;
    }

    dumpVar("DLVV", 0)


    quantum_qft(width, &t);
    QFT::ApplyQFT(*p, width);
    if (compare(p, &t)) {
        return;
    }

    dumpVar("AQFT", 0)
    int c = quantum_measure(t);
    int q = Measurer::Measure(*p);
    if (c != q) {
        fprintf(stderr, "BAYDA\n");
    }

    fprintf(stderr, "PASSED\n");
    delete p;

}

void tests::collapseTest() {
    fprintf(stderr, "COLLAPSE_TEST\n");
    IQRegister *p = new UserDefQRegister(width, 0);
    quantum_reg t = quantum_new_qureg(0, width);
    apply_walsh(*p);
    quantum_walsh(width, &t);
    int swidth = 6;
    quantum_addscratch(swidth, &t);
    p->allocSharedMem(swidth);
    for (int i = 0; i < swidth / 2; i++) {
        ApplyCnot(*p, i, width + i);
        quantum_cnot(i, width + i, &t);
    }


    for (int i = 0; i < swidth; i++) {
        quantum_bmeasure(0, &t);
    }

    QRegHelper::DeleteLocalVars(*p, swidth);

    if (compare(p, &t)) {
        ParallelLogger(stderr, "FAILED\n");
        return;
    }
    ParallelLogger(stderr, "PASSED\n");
}


/*void tests::expTest() {
    // not implemented yet
    fprintf(stderr, "EXPAMODN test\n");
    width =  4;
    int swidth = 2;
    IQRegister *p = new UserDefQRegister(width, 0);
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
    ParallelLogger(stderr, "QFTTEST\n");
    IQRegister *p = new UserDefQRegister(width, 2);
    ApplyToffoli(*p, 0, 1, 2);
    //log_print();
    quantum_reg t = quantum_new_qureg(2, width);
    apply_walsh(*p);
    //log_print();
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
    ParallelLogger(stderr, "TEST PASSED\n");
}


void tests::MeasureTest() {
    ParallelLogger(stderr, "MEASURETEST\n");
    //const int width = 4;
    width = 3;
    IQRegister *p = new UserDefQRegister(width, 2);
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
    state_t q = Measurer::Measure(*p);
    if (q != static_cast<state_t>(-1)) {
        ParallelLogger(stderr, "TEST PASSED\n");
    } else {
        ParallelLogger(stderr, "TEST FAILED\n");
    }
}




void tests::fake_test()
{

    ParallelLogger(stderr, "FAKE_TEST \n");

    IQRegister *p = new UserDefQRegister(3, 2);
    quantum_reg t = quantum_new_qureg(2, 3);
    apply_walsh(*p);
    quantum_walsh(3, &t);
    if (compare(p, &t)) {
        return;
    }
    ParallelLogger(stderr, "Walsh passed\n")
    const int SharedMem = 1;
    p->allocSharedMem(SharedMem);
    quantum_addscratch(SharedMem, &t);
    if (compare(p, &t)) {
        return;
    }
    ParallelLogger(stderr, "Shared passed\n")
    for (int i = 0; i < SharedMem; i++) {
        quantum_bmeasure(0, &t);
    }
    ParallelLogger(stderr, "libq Bmeasure passed\n")
    QRegHelper::DeleteLocalVars(*p, SharedMem);
    ParallelLogger(stderr, "Our Bmeasure passed\n")
            int drank = 3;
    dumpVar("BBBB", drank);
    dumpVar(p->getStatesSize(), drank)

    if (compare(p, &t)) {
        return;
    }
    ParallelLogger(stderr, "TEST PASSED\n");
}