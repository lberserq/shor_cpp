#include "config.h"
#include "common.h"
#include "qregister.h"
#include "qft.h"
#include "measure.h"
#include "multiplier.h"
#include "adder.h"
#include <cstdio>
#include <cstdlib>
#define GT
#define STATS_ONLY

#ifdef GT
#include "tests.h"
void gates_test() {
    tests::width = 4;
    tests::HadmardTest();
    tests::CNOTTest();
    tests::NotTest();
    tests::SwapTest();
    tests::ToffoliTest();
    tests::AdderTest();
    tests::MulTest();
    tests::expTest();
    tests::QFTTest();
}
#endif

state igcd(state a, state b) {
    while (a && b) {
        if (a > b) {
            a %= b;
        } else {
            b %= a;
        }
    }
    return (a) ? a : b;
}
state ipow(state base, int exp) {
    state res = 1;
    for (int i = 0; i < exp; i++, res *= base);
    return res;
}

int get_dim(state n) {
    state i = 0;
    while (static_cast<state>(1<< i) < n) {
        i++;
    }
    return i;
}



std::pair<state, state> con_frac_approx(long double val, long double maxdenum) {
    long long numerator = 1, denumerator = 0;
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
    MAX_TICK_COUNT = 100
};
/*!
 * \brief shor
 * \param n number to represent
 *
 *Algorithm Alloc mem and padding
 *Apply Uf
 *Apply QFT
 *Measure First reg
 *approximate mes_val / (1 << width) with elementary partition
 *use denominator as a period
 *
 */

void init_rand() {
    srand(time(NULL));
    srand48(time(NULL));
}

int shor(int n) {
    init_rand();
//    if (n == 18) {
//        n = 17 + 1;
//    }
    int all_width = get_dim(n * n);
    int width_local = get_dim(n);
    printf("Factorization of %d\n", n);
    fprintf(stderr, "w %d sw %d\n", all_width, width_local);
    int local_variables_size = 3 * width_local + 2;
    printf("We need %d qbits\n", all_width + 3 * width_local);
    if (all_width + 3 * width_local > 63) {
        printf("Too big number\n");
        return MAX_TICK_COUNT;
    }
    state m_state;
    int tick_count = 0;


    while (tick_count < MAX_TICK_COUNT) {
        tick_count++;
        IQRegister *tmp = new StaticQRegister(all_width, 0);
        for(int i = 0; i < all_width; i++) {
            ApplyHadamard(*tmp, i);
        }
        // Alloc mem for Uf
        tmp->printNorm();
        tmp->allocSharedMem(local_variables_size);

        int a = rand() % n;
        while (igcd(a, n) > 1 || a < 2) {
            a = rand() % n;
        }

        fprintf(stderr, "New Round %d of %d \n RANDOM NUMBER == %d\n", tick_count, MAX_TICK_COUNT, a);
        //Apply Uf
        expamodn(*tmp, n, a, all_width, width_local);
        tmp->printNorm();

        //stage1 delete local vars which we used, in Uf
        DeleteLocalVars(*tmp, local_variables_size);
        //stage2 Apply QFT on whole register(because it`s simplier, than use *** paddings)
        QFT::ApplyQFT(*tmp, all_width);
        tmp->printNorm();
        //Swap XY because measurer works only with X
        SwapXY(*tmp, all_width);
        tmp->printNorm();

        //Measure Y(now in X) with simple Measurer(basis is natural(like |0><0|)
        //m_state = Measurer::Measure(*tmp);

#ifdef LOGGING
        log_print();
        exit(0);
#endif

#ifdef STATS_ONLY
        printf("\n States count == %llu, Finished with %llu\n", tmp->getStatesSize(), m_state);
        tmp->print();
        tmp->printNorm();
        return -1;
#endif
        if (m_state == static_cast<state>(-1)) {
            std::fprintf(stderr, "Invalid Measurement\n");
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
        delete tmp;
        state del0 = ipow(a, frac.second / 2) + 1;

        state del1 = del0 - 2; // also == ipow(a, frac.second / 2) - 1;
        del0 = del0 % n;
        del1 = del1 % n;
        int div1 = igcd(n, del0);
        int div2 = igcd(n, del1);
        if (div1 > 1 && div1 != n) {
            fprintf(stderr, "Lucky round divisors `ve been found %d * %d == %d\n", n / div1, div1, n);
            return tick_count;
        } else if (div2 > 1 && div2 != n) {
            fprintf(stderr, "Lucky round divisors `ve been found %d * %d == %d\n", n / div2, div2, n);
            return tick_count;
        } else {
            fprintf(stderr, "Unlucky round\n");
        }

    }
    fprintf(stderr, "Cant find divisors with %d iterations, maybe %d is prime ?", MAX_TICK_COUNT, n);
    return tick_count;

}

void test() {
    srand(0);
    for (int i = 4;i < 21; i++) {
        shor(i);
    }
}

int main(int argc, char *argv[])
{
    gates_test();
    //return 0;

//    if (argc < 2) {
//        fprintf(stderr, "Usage %s Number\n", argv[0]);
//        return 1;
//    }
//    int n;
//    if (!sscanf(argv[1], "%d", &n)) {
//        fprintf(stderr, "%s is not a number\n", argv[1]);
//        return 1;
//    }
//    shor(n);
    //    freopen("scheme.txt", "w", stderr);
    //test();
    return 0;

}
