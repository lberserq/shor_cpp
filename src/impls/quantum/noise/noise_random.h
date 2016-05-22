#ifndef NOISE_RANDOM_H
#define NOISE_RANDOM_H
#if __cplusplus >= 201103L
#include <random>
#endif
namespace
{
#if __cplusplus < 201103L
    long  double xGenNRand() {
        const int N = 20;
        double sum = 0;
        for (int i = 0; i < N; i++) {
            sum += drand48();
        }
        return sum - N / 2;
    }
#else
    long double xGenNRand()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::normal_distribution<long double> d;
        return d(gen);
    }
#endif
}
#endif // NOISE_RANDOM_H

