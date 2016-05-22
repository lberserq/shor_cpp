#ifndef COMMONWORLD_H
#define COMMONWORLD_H
#include <iface/infra/icommonworld.h>
template <class IGatesProvider>
class CommonWorld: public ICommonWorld
{
    service_ptr_t<IGates> m_gatesprovider;
    service_ptr_t<INoiseProvider> m_noiseprovider;
public:
    CommonWorld(INoiseProvider *noiseProvider)
        : m_gatesprovider(service_ptr_t<IGates>(new IGatesProvider()))
        , m_noiseprovider(noiseProvider)
    {
    }


    IGates *GetGatesProvider() {
        return m_gatesprovider.get();
    }

    INoiseProvider *GetNoiseProvider() {
        return m_noiseprovider.get();
    }

};

#endif // COMMONWORLD_H
