#ifndef BINARYSERIALIZER_H
#define BINARYSERIALIZER_H
#include <iface/infra/iregserializer.h>

class BinaryRegSerializer :
        public IRegSerializer,
        public IRegDeserializer
{
public:
    result_t serialize(const IQRegister &in, CSerContext &context);
    result_t deserialize(IQRegister **out, CSerContext &context, IQRegisterFactory &factory);
};

#endif // BINARYSERIALIZER_H
