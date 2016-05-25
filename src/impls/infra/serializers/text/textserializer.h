#ifndef TEXTSERIALIZER_H
#define TEXTSERIALIZER_H
#include <iface/infra/iregserializer.h>

class TextSerializer :
        public IRegSerializer,
        public IRegDeserializer
{
public:
    result_t serialize(const IQRegister &in,  CSerContext &context);
    result_t deserialize(IQRegister **out,  CSerContext &context, IQRegisterFactory &factory);
};

#endif // TEXTSERIALIZER_H
