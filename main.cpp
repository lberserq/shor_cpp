#include "config.h"
#include "common.h"
#include "qregister.h"
#include "common_lib.h"
#include <cstdio>
#include <cstdlib>
#define GT
#define STATS_ONLY

#ifdef GT
#include "tests.h"

int test2() {
    IQRegister *tmp = new SharedQSimpleQRegister(4, 2);
    for (int i = 0;i < 4; i++) {
        ApplyHadamard(*tmp, i);
    }
    QRegHelpers::RegSwapLR(1, *tmp);
    return -1;

}

void gates_test() {
    tests::width = 3;
    tests::HadmardTest();
    tests::CNOTTest();
    tests::NotTest();
    tests::SwapTest();
    tests::ToffoliTest();
    tests::AdderTest();
    tests::MulTest();
    tests::expTest();
    tests::QFTTest();
    tests::MeasureTest();
    tests::fake_test();
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
    //const float interpolation_step = g_eps * 1e4 * 5;
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
    state m_state = 0;
    int tick_count = 0;


    while (tick_count < MAX_TICK_COUNT) {
        tick_count++;
        IQRegister *tmp = new UserDefQRegister(all_width, 0);
        for(int i = 0; i < all_width; i++) {
            ApplyHadamard(*tmp, i);
        }
        // Alloc mem for Uf
        q_log("WALSH");

        tmp->printNorm();
        tmp->allocSharedMem(local_variables_size);
        q_log("ALLOC SHMEM");


        MPI_Barrier(MPI_COMM_WORLD);
        int a = xGenIrand() % n;
        while (igcd(a, n) > 1 || a < 2) {
            a = xGenIrand() % n;
            q_log("GCD ITERATION");
        }
        MPI_Barrier(MPI_COMM_WORLD);

//        int a = n + 1;
        fprintf(stderr, "New Round %d of %d \n RANDOM NUMBER == %d\n", tick_count, MAX_TICK_COUNT, a);
        //Apply Uf
        q_log("IGCD");
        expamodn(*tmp, n, a, all_width, width_local);
        q_log("EXPAMODN");

        tmp->printNorm();

        //stage1 delete local vars which we used, in Uf
        QRegHelpers::DeleteLocalVars(*tmp, local_variables_size);
        q_log("DELLOCVARS");
        //stage2 Apply QFT on whole register(because it`s simplier, than use *** paddings)
        QFT::ApplyQFT(*tmp, all_width);
        q_log("QFT");
        tmp->printNorm();
        //Swap XY because measurer works only with X
        QRegHelpers::SwapXY(*tmp, all_width);
        q_log("SXY");
        tmp->printNorm();

        //Measure Y(now in X) with simple Measurer(basis is natural(like |0><0|)
        //m_state = Measurer::Measure(*tmp);

#ifdef LOGGING
        log_print();
        exit(0);
#endif

//#ifdef STATS_ONLY
//        printf("\n States count == %lu, Finished with %llu\n", tmp->getStatesSize(), m_state);
//        tmp->print();
//        tmp->printNorm();
//        return -1;
//#endif
        if (m_state == static_cast<state>(-1)) {
            std::fprintf(stderr, "Invalid Measurement\n");
            q_log("Invalid measurement")
            delete tmp;
            continue;
        }
        if (m_state == 0) {
            std::fprintf(stderr, "Zero measurement\n");
            q_log("Zero measurement");
            delete tmp;
            continue;
        }

        long double val = static_cast<double>(m_state) / static_cast<state>(1 << all_width);
        fprintf(stderr, "MESVAL == %llu / %llu \n", m_state, static_cast<state>(1 << all_width));
        std::pair<state, state> frac = con_frac_approx(val, static_cast<long double>(1 << all_width));
        fprintf(stderr, "SIMPLIFIER %llu / %llu\n", frac.first, frac.second);
        if (frac.second % 2 == 1) {
            fprintf(stdout, "Unfortunately, but denumerator is odd next round\n");
            q_log("Unfortunately, but denumerator is odd next round");

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
            q_log("Lucky round divisors `ve been found");
            q_log(n / div1);
            q_log(div1);
            q_log(n);

            return tick_count;
        } else if (div2 > 1 && div2 != n) {
            fprintf(stderr, "Lucky round divisors `ve been found %d * %d == %d\n", n / div2, div2, n);
            q_log("Lucky round divisors `ve been found");
            q_log(n / div2);
            q_log(div2);
            q_log(n);
            return tick_count;
        } else {
            fprintf(stderr, "Unlucky round\n");
            q_log("Unlucky round");

        }

    }
    fprintf(stderr, "Cant find divisors with %d iterations, maybe %d is prime ?", MAX_TICK_COUNT, n);

    q_log("Cant find divisors with");
    q_log(MAX_TICK_COUNT);
    q_log("iterations, maybe");
    q_log(n);
    q_log("is prime ?");

    return tick_count;

}

void shor_test() {
    srand(0);
    for (int i = 4;i < 21; i++) {
        shor(i);
    }
}

#include "xml_parsers.h"
void xml_test() {
    XmlParser parser("/home/lberserq/tmp/test.xml");
    std::vector<QMatrix> resV = parser.GetOperators(2);
    std::cout << resV[0].toString();
}

ICommonWorld *gWorld = NULL;
#include "common_impl.h"

INoiseProvider * CreateNoiseProvider(const std::string &filename)
{
    return CreateNoiseProviderStrDimImpl<NoNoiseImpl, CraussNoiseDensityImpl>(filename, 0);
}

void userInit() {
    MPI_Init(NULL, NULL);
    INoiseProvider *noiseProvider = CreateNoiseProvider("fname");
    gWorld = new CommonWorld<NUMAGatesImpl>(noiseProvider);
}


void userFinalize() {
    if (gWorld) {
        delete gWorld;
    }
    MPI_Finalize();
}

int main(int argc, char *argv[])
{
    xml_test();
    userInit();
        gates_test();
        userFinalize();
    return 0;

    if (argc < 2) {
        fprintf(stderr, "Usage %s Number\n", argv[0]);
    } else {
        int n;
        if (!sscanf(argv[1], "%d", &n)) {
            fprintf(stderr, "%s is not a number\n", argv[1]);
        } else {
            MPI_Barrier(MPI_COMM_WORLD);
            shor(n);
            MPI_Barrier(MPI_COMM_WORLD);
            freopen("scheme.txt", "w", stderr);
        }
        //test();
    }
    userFinalize();
    return 0;

}
