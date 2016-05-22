#include <limits.h>
#include <ctime>
#include <cstdlib>

double xGenDrand()
{
    static unsigned long long x = 179425019;
    const unsigned long long p = 179425019;
    const unsigned long long q = 179425027;
    const unsigned long long m = p *q;
    const unsigned long long ULRND_MAX  = (unsigned long long)(-1);
    x = (x * x) % m;
    return static_cast<double>(x) / (ULRND_MAX - 1);
}


int xGenIrand()
{
    static unsigned long long x = 179425020;
    const unsigned long long p = 179425019;
    const unsigned long long q = 179425027;
    const unsigned long long m = p *q;
//    const unsigned long long ULRND_MAX  =0xFFFFFFFFFFFFFFFF;
    x = (x * x) % m;
    return (int)    (x);
}

void initRand() {
    srand(time(NULL));
    srand48(time(NULL));
}
