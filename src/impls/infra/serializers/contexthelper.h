#ifndef CONTEXTHELPER_H
#define CONTEXTHELPER_H
#include <iface/infra/iregserializer.h>
#include <common/infra/io.h>



class contextIO
{
    CSerContext m_context;
    size_t m_rheaderSize;
    size_t m_wheaderSize;
    size_t m_pSize;
    ScopedMPIFile m_MPIFile;

    intptr_t m_handle;
public:
    contextIO(CSerContext &context);
    result_t read(byteArray &out, off64_t offset, uint64_t allSize);//APC
    result_t write(const byteArray &in, off64_t offset, uint64_t allSize);//APC

    result_t readHeader(byteArray &out); //IPC
    result_t writeHeader(const byteArray &in); //IPC
};


#endif // CONTEXTHELPER_H
