#ifndef REGSERIALIZATION_H
#define REGSERIALIZATION_H
#include <iface/infra/iregserializer.h>
#include <common/infra/impls_lists/quantum_register.h>
namespace serialization
{
    template <class InDeserializer, class OutSerializer>
    result_t convertRepresentation(CSerContext &context) {
        service_ptr_t<IRegDeserializer> deserializer(new InDeserializer());
        service_ptr_t<IRegSerializer> serializer(new OutSerializer());
        service_ptr_t<IQRegister> tmpRegister(new SharedSimpleQRegister(1, 0));

        if (serializer && deserializer && X_SUCCEEDED(deserializer->deserialize(tmpRegister, context)))
            return serializer->serialize(tmp, context);
        return X_FAIL;
    }


    template <class InDeserializer> result_t readFromFile(FILE *fin, IQRegister &out)
    {
        CSerContext serContext(fin);
        service_ptr_t<IRegDeserializer> deserializer(new InDeserializer());
        return deserializer->deserialize(out, serContext);
    }

    template <class InDeserializer> result_t readFromFile(int fd, IQRegister &out)
    {
        CSerContext serContext(fd);
        service_ptr_t<IRegDeserializer> deserializer(new InDeserializer());
        return deserializer->deserialize(out, serContext);
    }

    template <class InDeserializer> result_t readFromFile(const char *fileName, IQRegister &out)
    {
        CSerContext serContext(NULL, -1, fileName);
        service_ptr_t<IRegDeserializer> deserializer(new InDeserializer());
        return deserializer->deserialize(out, serContext);
    }

    template <class InDeserializer> result_t readFromMem(const std::vector<byte_t> &in, IQRegister &out)
    {
        CSerContext serContext(NULL, -1, NULL, in);
        service_ptr_t<IRegDeserializer> deserializer(new InDeserializer());
        return deserializer->deserialize(out, serContext);
    }


    template <class OutSerializer> result_t writeToFile(FILE *fout, IQRegister &in)
    {
        CSerContext serContext(fout);
        service_ptr_t<IRegSerializer> serializer(new OutSerializer());
        return serializer->serialize(in, serContext);
    }

    template <class OutSerializer> result_t writeToFile(int fd, IQRegister &in)
    {
        CSerContext serContext(fd);
        service_ptr_t<IRegSerializer> serializer(new OutSerializer());
        return serializer->serialize(in, serContext);
    }

    template <class OutSerializer> result_t writeToFile(const char *fileName, IQRegister &out)
    {
        CSerContext serContext(NULL, -1, fileName);
        service_ptr_t<IRegSerializer> serializer(new OutSerializer());
        return serializer->serialize(out, serContext);
    }

    template <class OutSerializer> result_t writeToMem(std::vector<byte_t> &out, IQRegister &in)
    {
        CSerContext serContext(NULL, -1, NULL, out);
        service_ptr_t<IRegSerializer> serializer(new OutSerializer());
        result_t result = serializer->serialize(in, serContext);
        if (X_SUCCEEDED(result))
            out = serContext.buffer;
        return result;
    }


}

#endif // REGSERIALIZATION_H

