#define STEST
#define SELFTEST

bool isFin = false;
#ifdef SELFTEST
#include <common/config.h>
#include <common/quantum/common.h>
#include <iface/quantum/iqregister.h>
#include <common/quantum_common_lib.h>
#include <cstdio>
#include <cstdlib>
#include <common/qmath.h>
#include <common/cvariant.h>
#include <common/infra/qscript_stubs.h>
#include <common/quantum_common_impl.h>
#define GT
#define STATS_ONLY

typedef SingleQRegister	UserDefQRegister;
#ifdef GT
#include "tests/tests.h"

int test2() {
    IQRegister *tmp = new SharedSimpleQRegister(4, 2);
    for (int i = 0;i < 4; i++) {
        ApplyHadamard(*tmp, i);
    }
    QRegHelper::RegSwapLR(1, *tmp);
    return -1;

}

void gates_test() {
    tests::width = 5;
    tests::HadmardTest();
    tests::CNOTTest();
    tests::NotTest();
    tests::SwapTest();
    tests::ToffoliTest();
    tests::AdderTest();
    tests::MulTest();
   // tests::expTest();
    tests::QFTTest();
    tests::MeasureTest();
    tests::fake_test();
    tests::collapseTest();
    tests::shor_test();
    tests::matrix_test();
    tests::crauss_test();
}
#endif

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


int shor(uint_type n) {
    initRand();
    //    if (n == 18) {
    //        n = 17 + 1;
    //    }
    int all_width = get_dim(n * n);
    int width_local = get_dim(n);
    printf("Factorization of %llu\n", n);
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


        ParallelSubSystemHelper::barrier();
        uint_type a = xGenIrand() % n;
        while (QMath::gcd(a, n) > 1 || a < 2) {
            a = xGenIrand() % n;
            q_log("GCD ITERATION");
        }
        ParallelSubSystemHelper::barrier();

//        int a = n + 1;
        fprintf(stderr, "New Round %d of %d \n RANDOM NUMBER == %llu\n", tick_count, MAX_TICK_COUNT, a);
        //Apply Uf
        q_log("IGCD");
        expamodn(*tmp, n, a, all_width, width_local);
        q_log("EXPAMODN");

        tmp->printNorm();

        //stage1 delete local vars which we used, in Uf
        QRegHelper::DeleteLocalVars(*tmp, local_variables_size);
        q_log("DELLOCVARS");
        //stage2 Apply QFT on whole register(because it`s simplier, than use *** paddings)
        QFT::ApplyQFT(*tmp, all_width);
        q_log("QFT");
        tmp->printNorm();
        //Swap XY because measurer works only with X
        QRegHelper::SwapXY(*tmp, all_width);
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
//        tmp->pючьrint();
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

        //long double val = static_cast<double>(m_state) / static_cast<state>(1 << all_width);
        fprintf(stderr, "MESVAL == %llu / %llu \n", m_state, static_cast<state>(1 << all_width));
        std::vector<cVariant> fracV = QMath::fracApprox(m_state, all_width, all_width);
        std::vector<uint_type> frac;
        for(size_t i = 0; i < fracV.size(); i++) {
            frac.push_back(static_cast<uint_type>(fracV[i].get<int_t>()));
        }
        fprintf(stderr, "SIMPLIFIER %llu / %llu\n", frac[0], frac[1]);
        if (frac[1] % 2 == 1) {
            fprintf(stdout, "Unfortunately, but denumerator is odd next round\n");
            q_log("Unfortunately, but denumerator is odd next round");

            delete tmp;
            continue;
        }
        delete tmp;
        uint_type del0 = QMath::gcd(static_cast<state>(a), frac[1] / 2) + 1;

        uint_type del1 = del0 - 2; // also == ipow(a, frac.second / 2) - 1;
        del0 = del0 % n;
        del1 = del1 % n;
        uint_type div1 = QMath::gcd(n, del0);
        uint_type div2 = QMath::gcd(n, del1);
        if (div1 > 1 && div1 != n) {
            fprintf(stderr, "Lucky round divisors `ve been found %llu * %llu == %llu\n", n / div1, div1, n);
            q_log("Lucky round divisors `ve been found");
            q_log(n / div1);
            q_log(div1);
            q_log(n);

            return tick_count;
        } else if (div2 > 1 && div2 != n) {
            fprintf(stderr, "Lucky round divisors `ve been found %llu * %llu == %llu\n", n / div2, div2, n);
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
    fprintf(stderr, "Cant find divisors with %d iterations, maybe %llu is prime ?", MAX_TICK_COUNT, n);

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

#include <common/infra/xmlparsers.h>
void xml_test() {
    q_log("XML_TEST");
    XmlParser parser("/home/lberserq/tmp/test.xml");
    std::vector<std::pair<QMatrix, std::vector<uint_type> > > resV = parser.GetOperators(1);
    q_log("operators_get");
    q_log(resV.size());

    //q_log(resV[0].first.toString());
}

service_ptr_t<ICommonWorld> gWorld;

INoiseProvider * CreateNoiseProvider(const std::string &filename)
{
    return CreateNoiseProviderStrDimImpl<NoNoiseImpl, NoiseDensityStub>(filename, 0);
}

void userInit() {
    MPI_Init(NULL, NULL);
    INoiseProvider *noiseProvider = CreateNoiseProvider("fname");
    gWorld = service_ptr_t<ICommonWorld>(new CommonWorld<MPIGatesImpl>(noiseProvider));
}


void userFinalize() {
    MPI_Finalize();
}

service_ptr_t<IQRegister> gRegister;
#include <common/infra/qscript_stubs.h>
int main(int argc, char *argv[])
{
    try
    {
        userInit();

        q_log("TESTS");

        xml_test();
        gates_test();
        userFinalize();
    }
    catch (const std::string & msg)
    {
        if (ParallelSubSystemHelper::isInited()) {
           dumpVar(msg, ParallelSubSystemHelper::getConfig().rank);
        } else {
            q_log(msg);
        }
    }
    catch (char const *msg)
    {
        if (ParallelSubSystemHelper::isInited()) {
           dumpVar(msg, ParallelSubSystemHelper::getConfig().rank);
        } else {
            q_log(msg);
        }
    }
    catch (const std::exception &ex) {
        if (ParallelSubSystemHelper::isInited()) {
           dumpVar(ex.what(), ParallelSubSystemHelper::getConfig().rank);
        } else {
            q_log(ex.what());
        }

    }
    //sQStubs::VectorSize<UserDefQRegister>(0);
    return 0;

    if (argc < 2) {
        fprintf(stderr, "Usage %s Number\n", argv[0]);
    } else {
        int n;
        if (!sscanf(argv[1], "%d", &n)) {
            fprintf(stderr, "%s is not a number\n", argv[1]);
        } else {
            ParallelSubSystemHelper::barrier();
            shor(n);
            ParallelSubSystemHelper::barrier();
        }
        //test();
    }
    userFinalize();
    return 0;

}

#else

#include <common/config.h>
#include <common/qmath.h>
#include <common/quantum_common_lib.h>
#include <common/infra/xml_parsers.h>
#include <common/quantum_common_impl.h>
#include <common/cvariant.h>
#include <common/infra/qscript_stubs.h>

service_ptr_t<ICommonWorld> gWorld;
service_ptr_t<IQRegister> gRegister;
	typedef SharedSimpleQRegister	UserDefQRegister;
uint_type measured_value;
	void UserInit() {
	initRand();
	MPI_Init(NULL, NULL);
#ifdef NO_NOISE
	gWorld = service_ptr_t<ICommonWorld>(new CommonWorld<MPIGatesImpl>(CreateNoiseProviderStrDimImpl<NoNoiseImpl, NoiseDensityStub>("/home/lberserq/tmp/test.xml", 4 )));
#else
#ifdef CR_NOISE
	gWorld = service_ptr_t<ICommonWorld>(new CommonWorld<MPIGatesImpl>(CreateNoiseProviderStrDimImpl<NoNoiseImpl, CraussNoiseDensityImpl>("/home/lberserq/tmp/test.xml", 1)));
#endif

#ifdef UN_NOISE
	gWorld = service_ptr_t<ICommonWorld>(new CommonWorld<MPIGatesImpl>(CreateNoiseProviderStrDimImpl<CRotNoiseImpl, NoiseDensityStub>("/home/lberserq/tmp/test.xml", 4 )));
#endif


#endif
        ParallelSubSystemHelper::barrier();
}
void FindFactors(const cVariant & N_ ) {
 cVariant N= N_;

cVariant x = 0;
std::vector<cVariant> matrixData;
matrixData.push_back(  1 );
matrixData.push_back(  2 );
matrixData.push_back(  3 );
matrixData.push_back(  4 );

QMatrix m= QStubs::DeclareMatrix4(matrixData);
cVariant width= QMath::getWidth(N);
cVariant gwidth= QMath::getWidth(N * N);
cVariant twidth= 2 * width + 3;

if (N < 4) {
    x = N - 1;
}
else  {
    x = 0;
    for(  x; (QMath::gcd(N.get<int_t>(), x.get<int_t>()) > 1) || (x < 2); x ) {
    x = QMath::floor(QMath::random() * 10000) % N.get<int_t>();
    }
}

cVariant a= 1;


QStubs::PrintStub(cVariant(  "Random seed: " ).toString()+ cVariant(  x).toString());

for(  cVariant i= 0; i < gwidth; i++ ) {
    q_log(i.get<int_t>());
QStubs::Hadamard(  i );
}

QStubs::ExpModN( x, N, twidth);

QStubs::QFT(  0, gwidth );

for(  cVariant i= 0; i < width / 2; i++ ) {
QStubs::Swap(  i, width - i - 1 );
}

#ifdef STEST
QStubs::PrintReg();
return;
#endif
for(  cVariant trycnt= 100; trycnt >= 0; trycnt-- ) {
cVariant measured_value= Measurer::Measure(*gRegister.get());
cVariant c= measured_value;

if(  c == 0 ) {
QStubs::PrintStub(cVariant(  "Measured zero, try again.").toString());
continue;
}

cVariant q= 1 << width;

QStubs::PrintStub(cVariant(  "Measured " ).toString()+ cVariant(  c ).toString()+ cVariant(  " (" ).toString()+ cVariant(  c / q ).toString()+ cVariant(  ")").toString());

cVariant tmp= QMath::fracApprox(c, q, width);

c = tmp[0];
q = tmp[1];

QStubs::PrintStub(cVariant(  "fractional approximation is " ).toString()+ cVariant(  c ).toString()+ cVariant(  "/" ).toString()+ cVariant(  q).toString());

if(  (q.get<int_t>() % 2 == 1) && (2 * q.get<int_t>() < (1 << width.get<int_t>())) ) {
QStubs::PrintStub(cVariant(  "Odd denominator, trying to expand by 2.").toString());
q *= 2;
}

if(  q.get<int_t>()  % 2 == 1 ) {
QStubs::PrintStub(cVariant(  "Odd period, try again.").toString());
continue;
}

QStubs::PrintStub(cVariant(  "Possible period is " ).toString()+ cVariant(  q).toString());

a = QMath::ipow(x.get<int_t>() , q.get<int_t>()  / 2) + 1 % N.get<int_t>() ;
cVariant b= QMath::ipow(x.get<int_t>() , q.get<int_t>()  / 2) - 1 % N.get<int_t>() ;

a = QMath::gcd(N.get<int_t>() , a.get<int_t>() );
b = QMath::gcd(N.get<int_t>() , b.get<int_t>() );

cVariant factor= a;
if(  a > b ) {
factor = a;
}
else
{
factor = b;
}

if(  (factor < N) && (factor > 1) ) {
QStubs::PrintStub(cVariant(  "<h2>Success: " ).toString()+ cVariant(  factor ).toString()+ cVariant(  " " ).toString()+ cVariant(  N / factor).toString());
std::abort();
}
else
{
QStubs::PrintStub(cVariant(  "Unable to determine factors, try again.").toString());
continue;
}
}
}
void UserFinalize()
{
MPI_Finalize();
}

#include <cmath>
namespace pell
{
    std::vector<real_t> gammas;
    int tau(int a, int b) {
        return (a % (2 * b));
    }



    double getGamma(const std::pair<int, int> &in, int D)
    {
        return (in.second + std::sqrt(D))/ (2 * in.first);
    }

    std::pair<int, int> applyRedRo(const std::pair<int, int> &in, int D) {
        //a == first
        //b == second

        int a = in.first;
        int b = in.second;

        int c = std::abs(D - b*b)/(4*a);
        gammas.push_back(std::abs(getGamma(in, D)));

        return std::make_pair(c, tau(-b, c));
    }

    double deltaStep(const std::pair<int, int> &, int )
    {
        double sum = 0;
        for (size_t i = 0; i < gammas.size(); i++) {
            sum += std::log(gammas[i]);
        }
        return sum;
    }

    int gcdE(int a, int b, int &x, int &y)
    {
        if (b == 0) {
            x = 1;
            y = 0;
            return a;
        }

        int x_prev = 0;
        int y_prev = 1;
        int x_curr = 1;
        int y_curr = 0;

        while (b > 0) {
            int q = a / b;
            int r = a - q * b;
            a = b;
            b = r;

            x = x_curr - q * x_prev;
            y = y_curr - q * y_prev;
            x_curr = x_prev;
            x_prev = x;
            y_curr = y_prev;
            y_prev = y;
        }
        x = x_curr;
        y = y_curr;
        return a;
    }
    //x1*a1+y1*a2=k0
    //x2*k0+y2*d = k1

    //x1*x2*a1+ x2*y1*a2+
    std::pair<int, int> multiply(const std::pair<int, int> &id1, const std::pair<int, int> &id2, int delta)
    {
        int a1 = id1.first;
        int a2 = id2.first;
        int b1 = id1.second;
        int b2 = id2.second;
        int p1, p2, q1, q2;
        int gg = gcdE(a1, a2, p1, p2);
        int k = gcdE(gg, (b1 + b2)/ 2, q1, q2);
        int u = p1 * q1;
        int v = p2 * q2;
        int w = q2;

        int a3 = (a1*a2/(k * k));
        int b3 = tau(u * a1 * b2 + v * a2 * b1 + w * (b1*b2 + delta)/2, a3);
        return std::make_pair(a3, b3);
    }
}

std::pair< std::pair<int, int>, double>  h(int x, int d)
{
    typedef std::pair<int, int> ideal;
    int delta = (d % 4 == 1) ? d :  4 * d;

    ideal id(pell::tau(delta, 2), 1);//
    id = pell::applyRedRo(id, delta);
    id = pell::applyRedRo(id, delta);
    std::vector<ideal> ideals;
    ideals.push_back(id);
    std::vector<real_t> deltas;
    double current_delta = pell::deltaStep(id, delta);
    while (!(current_delta > x)) {
        ideals.push_back(id);
        deltas.push_back(current_delta);

        id = pell::multiply(id, id, delta);
        double prev_Delta = current_delta;
        current_delta = prev_Delta + prev_Delta;
    }

    id = ideals.back();
    current_delta = deltas.back();
    ideals.pop_back();
    deltas.pop_back();

    while (std::abs(current_delta - x) > g_eps && current_delta < x && ideals.size())
    {
        ideal local_ideal = pell::multiply(id, ideals.back(), delta);
        real_t local_delta = current_delta + deltas.back();

        double prev_Delta = 0;
        std::vector<ideal> currentV;
        while (!(local_delta > x) && std::abs(current_delta - x) >= g_eps) {
            currentV.push_back(local_ideal);
            local_ideal = pell::multiply(local_ideal, ideals.back(), delta);
            prev_Delta = current_delta;
            current_delta = prev_Delta + deltas.back();
        }
        id = local_ideal;
        if (std::abs(current_delta - x) <= g_eps) {
            continue;
        }
        id = currentV.back();
        current_delta = prev_Delta;
        ideals.pop_back();
        deltas.pop_back();
    }

    double prev_delta = 0;
    ideal prev_ideal;
    do
    {
        prev_ideal = id;
        id = pell::applyRedRo(id, delta);
        id = pell::applyRedRo(id, delta);
        prev_delta = current_delta;
        current_delta += std::log(pell::gammas.back()) + std::log(pell::gammas[pell::gammas.size() - 2]);
    }
    while(!(current_delta > x));
    current_delta = prev_delta;
    id = prev_ideal;

    ideal idro  = pell::applyRedRo(id, delta);
    real_t delta_ro =  current_delta + std::log(pell::gammas.back());
    if (delta_ro < x) {
        id = idro;
        current_delta = delta_ro;
    }

    pell::gammas.clear();
    return std::make_pair(id, x - current_delta);
}

namespace
{
    int getBitMask(uint8_t size)
    {
        int mask = 0;
        for (int i = 0; i < size; ++i) {
            mask |= (1 << i);
        }
        return mask;
    }
}

#include <set>

void classicRegCalc(const cVariant &D, uint8_t nbitsRepr, uint8_t preciseBitsInt)
{

    int preciseMask = getBitMask(preciseBitsInt);
    int unPreciseMask = getBitMask(nbitsRepr);
    std::vector<mcomplex> ampls(gRegister->getStatesSize(), 0.0);
    std::set<size_t> m_set;

    for (size_t i = 0; i < static_cast<size_t>(1 << gRegister->getWidth()); i++)
    {
        size_t half_width = gRegister->getWidth() / 2;
        std::pair< std::pair<int, int>, double> hf = h(i + gRegister->getOffset(), D.get<int_t>());
        size_t right_value = (hf.first.first & preciseMask << preciseBitsInt) | (hf.first.second & preciseMask);
        size_t val = *reinterpret_cast<size_t *>(&hf.second);
        right_value = (right_value << nbitsRepr) | (val & unPreciseMask);

        size_t id = (i + gRegister->getOffset()) << half_width | right_value;

        m_set.insert(id);

        if (id >= gRegister->getOffset() &&
                id < gRegister->getOffset() + gRegister->getStatesSize()) {
            ampls[id - gRegister->getOffset()] = 1.0;
        }

        size_t pseudo_s_size = static_cast<size_t>(1 << gRegister->getWidth());



        if ( gRegister->getRepresentation() == MATRIX_REPRESENTATION
             && id + pseudo_s_size >= gRegister->getOffset()
             && id + pseudo_s_size < gRegister->getOffset() + gRegister->getStatesSize()) {
            ampls[id + pseudo_s_size - gRegister->getOffset()] = 1.0;
        }
    }

    mcomplex ampl = (1.0 / std::sqrt(m_set.size()));
    for (size_t i = 0; i < ampls.size(); ++i) {
        if (std::abs(ampls[i]) > g_eps) {
            ampls[i] = ampl;
        }
    }
    gRegister->setStates(ampls);
}


void PellEquation(const cVariant &d)
{
    uint8_t nbitsRepr = 5;
    uint8_t preciseBitsInt = 4;
    classicRegCalc(d, nbitsRepr, preciseBitsInt);
    int half_width = gRegister->getWidth() / 2;
    QStubs::QFT(0,  half_width);
    QStubs::PrintReg();
}


int main()
{
 UserInit();
try {
const int size = 8;
#ifndef CR_NOISE
QStubs::VectorSize<UserDefQRegister>( size, REG_REPRESENTATION);
#else
QStubs::VectorSize<UserDefQRegister>( size, MATRIX_REPRESENTATION);
#endif
#define NO_TEST
#ifndef NO_TEST
FindFactors( 5 );
#else
    const int N = size;
    for (int i = 0; i < N; i++) {
        QStubs::Hadamard(i);

    }

//    const int_t alpha = gRegister->getRepresentation() == REG_REPRESENTATION ? 1 : 2;
//    std::vector<complex_t> ampls(gRegister->getStatesSize());
//    for (size_t i = 0; i < gRegister->getStatesSize(); i++) {
//        const real_t gamma = 1.0 / (gRegister->getStatesSize() /2 * alpha);
//        real_t ampl = (i % 2 == 0) ? gamma : 0;
//        ampls[i] = ampl;
//    }
//    gRegister->setStates(ampls);

//    for (int i = 0; i < N / 2 - 1; i++) {
//        QStubs::CNOT(i, i + N / 2 + 1);
//    }
//    size_t width = gRegister->getWidth();
//    size_t nre = 4;

 //   classicRegCalc(2, nre, width - nre);
    //QStubs::PrintReg();

    QStubs::QFT(0, N);
    isFin = true;
    gWorld->GetNoiseProvider()->GetDensityMatrixNoise()->ApplyNoiseForDensityMatrix(*gRegister);

    QStubs::PrintReg();
#endif
 }
 catch (const std::string & msg)
 {
     if (ParallelSubSystemHelper::isInited()) {
        dumpVar(msg, ParallelSubSystemHelper::getConfig().rank);
     } else {
         q_log(msg);
     }
 }
 catch (char const *msg)
 {
     if (ParallelSubSystemHelper::isInited()) {
        dumpVar(msg, ParallelSubSystemHelper::getConfig().rank);
     } else {
         q_log(msg);
     }
 }
 catch (const std::exception &ex) {
     if (ParallelSubSystemHelper::isInited()) {
        dumpVar(ex.what(), ParallelSubSystemHelper::getConfig().rank);
     } else {
         q_log(ex.what());
     }

 }

UserFinalize();
}

#endif
