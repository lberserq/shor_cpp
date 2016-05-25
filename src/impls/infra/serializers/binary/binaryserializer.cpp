#include "binaryserializer.h"
#include <common/infra/parallelsubsystemcommon.h>
#include "../contexthelper.h"
#include "../serializers_common.h"

enum
{
    BINARY_MAGIC = 0x62726765,//breg
    BINARY_MAGIC_LEN = 4
};



result_t BinaryRegSerializer::serialize(const IQRegister &in, CSerContext &context)
{
    if (!in.getStatesSize())
        return X_SOK;

    contextIO io(context);

    struct RegHeader header;
    header.magic = BINARY_MAGIC;
    header.regWidth = in.getWidth() *
                      (1 + in.getRepresentation() == MATRIX_REPRESENTATION);
    header.regMode = serialization::helpers::ModeFromRepresentation(in.getRepresentation());
    header.regSize = static_cast<uint64_t>(1ll << header.regWidth);

    result_t writeHdrResult = serialization::helpers::WriteHeader(io, header);

    if (X_FAILED(writeHdrResult))
        return writeHdrResult;


    byteArray array(in.getStatesSize() * sizeof(mcomplex));
    memcpy(&array[0], &in.getStates()[0], array.size());


    off64_t off = 0;
    if (ParallelSubSystemHelper::isInited())
    {
        uint64_t baseblock = header.regSize / ParallelSubSystemHelper::getConfig().size;
        off = (ParallelSubSystemHelper::getConfig().rank - 1) * baseblock;
    }

    result_t writeResult = X_SOK;
    X_SAFE_COMMON_CALL_ALL(io.write(array, off, header.regSize), writeResult);
    return writeResult;
}

result_t BinaryRegSerializer::deserialize(IQRegister **out, CSerContext &context, IQRegisterFactory &factory)
{
    contextIO io(context);
    RegHeader hdr;
    result_t readHdrResult = serialization::helpers::ReadHeader(io, hdr);

    if (X_FAILED(readHdrResult))
        return readHdrResult;


    size_t localSize = hdr.regSize;
    off64_t off = 0;
    if (ParallelSubSystemHelper::isInited())
    {
        localSize = hdr.regSize /  ParallelSubSystemHelper::getConfig().size;
        off = (ParallelSubSystemHelper::getConfig().rank - 1) * localSize;

        if (ParallelSubSystemHelper::getConfig().rank ==
                ParallelSubSystemHelper::getConfig().size - 1)
            localSize = hdr.regSize - ParallelSubSystemHelper::getConfig().rank * localSize;
    }

    byteArray array(localSize * sizeof(mcomplex));
    result_t readResult = X_SOK;

    X_SAFE_COMMON_CALL_ALL(io.read(array, off, hdr.regSize), readResult);
    if (X_FAILED(readResult))
        return readResult;

    std::vector<mcomplex> ampls(localSize);
    memcpy(&ampls[0], &array[0], array.size());

    representation_t representation = serialization::helpers::RepresentationFromMode(hdr.regMode);

    result_t createResult = X_SOK;
    X_SAFE_COMMON_CALL_ALL(factory.CreateRegister(hdr.regWidth, 0, 1.0, representation, out), createResult);

    if (X_FAILED(createResult)) {
        return createResult;
    }

    (*out)->setStates(ampls);
    return X_SOK;
}
