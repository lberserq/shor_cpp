#include "config.h"
#include "common.h"
#include "qregister.h"
#include "qft.h"
#include "measure.h"
#include "multiplier.h"
#include "adder.h"
#include <cstdio>
#include <cstdlib>
#include "tests.h"
void gates_test() {

    tests::width = 10;
    tests::NotTest();
    tests::CNOTTest();
    //tests::ToffoliTest();
    /*Qregister t(2, 1);
    t.getAmpls()[0] = 1.0f;
    t.getStates()[0] = 1;
    ApplyHadamard(t, 0);
    for (int i = 0; i < t.getSize(); i++) {
        int st = t.getStates()[i];
        mcomplex p  = t.getAmpls()[i];
        double lf = p.real();
        fprintf(stderr, "%lf %x\n",lf, st);
    }*/
//    const int width = 4;
//    const int N = 100;
//    Qregister *tmp = new Qregister(width, 2);
//    ApplyHadamard(*tmp, 0);
//    fprintf(stderr, "After hadamard\n");
//    tmp->print();
//    int i = 0;
//    Adder *add = new Adder(i, N, width, *tmp);
//    add->perform();
//    //tmp->print();
//    tests::width = 4;
//    fprintf(stderr, "AUTO_TEST\n");
    tests::AdderTest();
    tests::MulTest();
    tests::QFTTest();
    tests::MeasureTest();
    tests::SwapTest();
    tests::collapseTest();
    tests::expTest();


}

int igcd(int a, int b) {
    while (a && b) {
        if (a > b) {
             a %= b;
        } else {
            b %= a;
        }
    }
    return (a) ? a : b;
}
int ipow(int base, int exp) {
    int res = 1;
    for (int i = 0; i < exp; i++, res *= base);
    return res;
}

int get_dim(int n) {
    int i = 0;
    while ((1<< i) < n) {
        i++;
    }
    return i;
}



std::pair<state, state> con_frac_approx(long double val, long double maxdenum) {
    state numerator = 1, denumerator = 0;
    state numlast = 0, denumlast = 1;
    const float interpolation_step = g_eps * 1e4 * 5;
    long double ai = val;
    long double step = 0.0f;
    do {
        step = ai + 0.000005;
        step = std::floor(step);
        ai -= step - 0.0010005;
        ai = 1.0f / ai;
        if (step * denumerator + denumlast > maxdenum) {
            break;
        }
        state  savenum = numerator;
        state savedenum = denumerator;
        numerator = step * numerator + numlast;
        denumerator = step * denumerator + denumlast;
        numlast = savenum;
        denumlast = savedenum;
    }while(std::fabs(static_cast<long double>(numerator) / denumerator - val) > 1.0 / (2.0 * maxdenum));


    if (numerator < 0 && denumerator < 0) {
        numerator = -numerator;
        denumerator = -denumerator;
    }
    std::pair<state, state> res = std::make_pair(numerator, denumerator);
    return res;
}



enum
{
    MAX_TICK_COUNT = 10000
};


void shor(int n) {
    int all_width = get_dim(n * n);
    int width_local = get_dim(n);
    fprintf(stderr, "w %d sw %d\n", all_width, width_local);
    int local_variables_size = 3 * width_local + 2;
    state m_state;
    int tick_count = 0;
    while (tick_count < MAX_TICK_COUNT) {
        Qregister *tmp = new Qregister(all_width, 0);
        for(int i = 0; i < all_width; i++) {
            ApplyHadamard(*tmp, i);
        }
       // Alloc mem for Uf
        tmp->alloc_smem(local_variables_size);


        int a = rand() % n;
        while (igcd(a, n) > 1 || a < 2) {
            a = rand() % n;
        }
        if (n == 20) {
            a = 17;
        }
        fprintf(stdout, "New Round\n RANDOM NUMBER == %d\n", a);
        //Apply Uf
        expamodn(*tmp, n, a, all_width, width_local);
//        fprintf(stderr, "after expamodn\n");
//        tmp->print();
//        return;
//
        //stage1 delete local vars which we used, in Uf
        DeleteLocalVars(*tmp, local_variables_size);
        //stage2 Apply QFT on whole register(because it`s simplier, than use *** paddings)
        QFT::ApplyQft(*tmp, all_width);

        //Swap XY because measurer works only with X
        SwapXY(*tmp, all_width);


        //Measure Y(now in X) with simple Measurer(basis is natural(like |0><0|)
        m_state = Measurer::Measure(*tmp);
        if (m_state == static_cast<state>(-1)) {
            std::fprintf(stdout, "Invalid Measurement\n");
            delete tmp;
            continue;
        }
        if (m_state == 0) {
            std::fprintf(stderr, "Zero measurement\n");
            delete tmp;
            continue;
        }

        long double val = static_cast<double>(m_state) / static_cast<state>(1 << all_width);
        fprintf(stderr, "MESVAL == %llu / %llu \n", m_state, static_cast<state>(1 << all_width));
        std::pair<state, state> frac = con_frac_approx(val, static_cast<long double>(1 << all_width));
        fprintf(stderr, "SIMPLIFIER %llu / %llu\n", frac.first, frac.second);
        if (frac.second % 2 == 1) {
            fprintf(stdout, "Unfortunately, but denumerator is odd next round\n");

            delete tmp;
            continue;
        }
        int del0 = ipow(a, frac.second / 2) + 1;

        int del1 = del0 - 2; // also == ipow(a, frac.second / 2) - 1;
        del0 = del0 % n;
        del1 = del1 % n;
        int div1 = igcd(n, del0);
        int div2 = igcd(n, del1);
        if (div1 > 1 && div1 != n) {
            fprintf(stdout, "Lucky round divisors `ve been found %d * %d == %d\n", n / div1, div1, n);
            return;
        } else if (div2 > 1 && div2 != n) {
            fprintf(stdout, "Lucky round divisors `ve been found %d * %d == %d\n", n / div2, div2, n);
            return;
        } else {
            fprintf(stdout, "Unlucky round\n");
        }

    }
    fprintf(stdout, "Cant find divisors with %d iterations, maybe %d is prime ?", MAX_TICK_COUNT, n);
}

int main(int argc, char *argv[])
{
    gates_test();
    //return 0;
    shor(15);
    if (argc < 2) {
        fprintf(stderr, "Usage %s Number\n", argv[0]);
        return 1;
    }
    int n;
    if (!sscanf(argv[1], "%d", &n)) {
        fprintf(stderr, "%s is not a number\n", argv[1]);
        return 1;
    }
    shor(n);

}
