#ifndef SERIALIZERS_COMMON_H
#define SERIALIZERS_COMMON_H
#include <stdint.h>
#include "contexthelper.h"
struct RegHeader
{
    int32_t magic;
    uint64_t regSize;
    uint32_t regWidth;
    uint8_t regMode;
};

namespace serialization
{
namespace helpers
{

    template <class Header> inline
    result_t ReadHeader(contextIO &io, Header &hdr)
    {
        byteArray headerBin(sizeof(hdr));
        result_t readHdrResult = X_SOK;

        if (!ParallelSubSystemHelper::isInited()
                || !ParallelSubSystemHelper::getConfig().rank)
        {
            readHdrResult = io.readHeader(headerBin);
        }

        if (ParallelSubSystemHelper::isInited())
        {
            MPI_SAFE_CALL(MPI_Bcast(&readHdrResult, 1, MPI_RESULT_T, 0, MPI_COMM_WORLD));
        }

        if (X_FAILED(readHdrResult))
            return readHdrResult;

        memcpy(&hdr, &headerBin[0], headerBin.size());
        return X_SOK;
    }

    template <class Header> inline
    result_t WriteHeader(contextIO &io, const Header &hdr)
    {
        byteArray headerBin(sizeof(hdr));
        memcpy(&headerBin[0], &hdr, headerBin.size());

        result_t writeHdrResult = X_SOK;

        if (!ParallelSubSystemHelper::isInited()
                || !ParallelSubSystemHelper::getConfig().rank)
        {
            writeHdrResult = io.writeHeader(headerBin);
        }

        if (ParallelSubSystemHelper::isInited())
        {
            MPI_SAFE_CALL(MPI_Bcast(&writeHdrResult, 1, MPI_RESULT_T, 0, MPI_COMM_WORLD));
        }

        return writeHdrResult;
    }



    inline uint8_t ModeFromRepresentation(representation_t representation)
    {
        return static_cast<uint8_t>(representation);
    }


    inline representation_t RepresentationFromMode(uint8_t mode)
    {
        return static_cast<representation_t>(mode);
    }


}//helpers
}//serialization

#endif // SERIALIZERS_COMMON_H

