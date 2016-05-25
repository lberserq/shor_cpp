#ifndef IREGSERIALIZER_H
#define IREGSERIALIZER_H
#include <iface/quantum/iqregister.h>
/*!
 * \brief The cSerContext struct
 *      Serialization context with data paths or descriptors
 *      Serializer shuld use data
 */
#include <common/config.h>
#include <vector>
struct CSerContext
{
    enum mode_t
    {
        SER_DATA_C_FILE,
        SER_DATA_FD,
        SER_DATA_FNAME,
        SER_DATA_BUFFER,
        SER_DATA_UNDEFINED,
    };

    FILE *fin;
    int fd;
    const char *filename;
    byteArray buffer;
    CSerContext(FILE *fin = NULL,
                int fd = -1,
                const char *filename = NULL,
                const byteArray &buffer = byteArray())
        : fin(fin),
          fd(fd),
          filename(filename),
          buffer(buffer)
    {}

    mode_t  getMode() const
    {
        if (fin)
            return SER_DATA_C_FILE;
        if (fd != -1)
            return SER_DATA_FD;
        if (filename)
            return SER_DATA_FNAME;
        if (buffer.size())
            return SER_DATA_BUFFER;

        return SER_DATA_UNDEFINED;
    }
};

struct IRegSerializer
{
    /*!
     * \brief serialize serialize register by context
     * \param in  register
     * \param context
     * \return X_OK if ok, error code otherwise
     */
    virtual result_t serialize(const IQRegister &in,  CSerContext &context) = 0;
    virtual ~IRegSerializer(){}
};
struct IRegDeserializer
{
    /*!
     * \brief deserialize register by context
     * \param out  register
     * \param context
     * \param factory register factory
     * \return X_OK if ok, error code otherwise
     */
    virtual result_t deserialize(IQRegister **out,  CSerContext &context, IQRegisterFactory &factory) = 0;
    virtual ~IRegDeserializer(){}
};


#endif // IREGSERIALIZER_H

