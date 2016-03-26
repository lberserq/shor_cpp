#ifndef ICOMMONWORLD_H
#define ICOMMONWORLD_H
#include <inoise.h>
#include <igates.h>
struct ICommonWorld
{
    virtual IGates * GetGatesProvider() = 0;
    virtual INoiseProvider * GetNoiseProvider() = 0;
    virtual ~ICommonWorld(){}
};


#endif // ICOMMONWORLD_H

