#ifndef COMMONWORLD_H
#define COMMONWORLD_H
#include <icommonworld.h>
template <class IGatesProvider>
class CommonWorld: public ICommonWorld
{
    IGates *m_gatesprovider;
    INoiseProvider *m_noiseprovider;
public:
    CommonWorld(INoiseProvider *noiseProvider) : m_noiseprovider(noiseProvider){
        m_gatesprovider = new IGatesProvider();
    }

    IGates *GetGatesProvider() {
        return m_gatesprovider;
    }

    INoiseProvider *GetNoiseProvider() {
        return m_noiseprovider;
    }

    ~CommonWorld() {
        delete m_gatesprovider;
        delete m_noiseprovider;
    }

};

#endif // COMMONWORLD_H
