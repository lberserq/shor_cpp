#include "textserializer.h"
#include "../serializers_common.h"
#include <common/infra/parallelsubsystemcommon.h>
#include <sstream>
#include <iomanip>
enum
{
    TEXT_MAGIC = 0x74726567
};

const int text_w = 15;

result_t TextSerializer::serialize(const IQRegister &in, CSerContext &context)
{
    if (!in.getStatesSize())
        return X_SOK;

    contextIO io(context);

    struct RegHeader header;
    header.magic = TEXT_MAGIC;
    header.regWidth = in.getWidth() *
                      (1 + in.getRepresentation() == MATRIX_REPRESENTATION);
    header.regMode = serialization::helpers::ModeFromRepresentation(in.getRepresentation());
    header.regSize = static_cast<uint64_t>(1ll << header.regWidth);

    result_t writeHdrResult = serialization::helpers::WriteHeader(io, header);

    if (X_FAILED(writeHdrResult))
        return writeHdrResult;


    std::stringstream sstream;
    sstream << std::fixed << std::setprecision(4);
    for (size_t i = 0; i < in.getStatesSize(); ++i) {
        sstream <<  std::setw(text_w) << in.getStates()[i] << std::endl;
    }

    std::string strmStr = sstream.str();
    const byte_t *begin = reinterpret_cast<const byte_t *>(strmStr.c_str());
    byteArray array(reinterpret_cast<const byte_t *>(begin),  reinterpret_cast<const byte_t *>(begin + strmStr.size()));

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

inline result_t readStream(std::stringstream &in, std::vector<mcomplex> &out)
{
    try
    {
        for (size_t i = 0; i < out.size(); ++i) {
            in >> out[i];
        }
        return X_SOK;
    }
    catch(std::exception &e)
    {
        if (ParallelSubSystemHelper::isInited())
        {
            q_log_always_impl(e.what(), ParallelSubSystemHelper::getConfig().rank)
        }
        else
            q_log(e.what());
        return X_FAIL;
    }
}


result_t TextSerializer::deserialize(IQRegister **out, CSerContext &context, IQRegisterFactory &factory)
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

    byteArray array(localSize * (text_w + 1));
    result_t readResult = X_SOK;

    X_SAFE_COMMON_CALL_ALL(io.read(array, off, hdr.regSize), readResult);
    if (X_FAILED(readResult))
        return readResult;

    std::vector<mcomplex> ampls(localSize);

    std::string str(reinterpret_cast<char *>(&array[0]), reinterpret_cast<char *>(&array[0] + array.size()));
    std::stringstream sstream(str);


    result_t parseResult = X_SOK;
    X_SAFE_COMMON_CALL_ALL(readStream(sstream, ampls), parseResult);
    if (X_FAILED(parseResult))
        return parseResult;

    representation_t representation = serialization::helpers::RepresentationFromMode(hdr.regMode);

    result_t createResult = X_SOK;
    X_SAFE_COMMON_CALL_ALL(factory.CreateRegister(hdr.regWidth, 0, 1.0, representation, out), createResult);

    if (X_FAILED(createResult)) {
        return createResult;
    }

    (*out)->setStates(ampls);
    return X_SOK;
}
