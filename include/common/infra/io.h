#ifndef IO_H
#define IO_H
#include <common/infra/mpiio.h>
#include <cstdio>
#include <unistd.h>
namespace deleters
{
    template <> inline void deleter<FILE *>(FILE *& fin)
    {
        fclose(fin);
    }

    template<> inline void deleter<int>(int &fd)
    {
        close(fd);
    }
}


typedef ScopedHandle<int> ScopedFD;
typedef ScopedHandle<FILE *> ScopedCFile;
#endif // IO_H

