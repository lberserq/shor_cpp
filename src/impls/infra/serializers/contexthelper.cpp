#include "contexthelper.h"


contextIO::contextIO(CSerContext &context)
    :
      m_rheaderSize(0),
      m_wheaderSize(0),
      m_pSize(0),
      m_MPIFile(NULL),
      m_handle(0)
{
    m_context.fd = context.fd;
    m_context.filename = context.filename;
    m_context.fin = context.fin;

    switch (context.getMode())
    {
    case CSerContext::SER_DATA_BUFFER:
        X_ASSERT(context.buffer.size());
        m_handle = reinterpret_cast<intptr_t>(&context.buffer[0]);
        m_pSize = context.buffer.size();
        m_context.buffer.resize(1);
        break;
    case CSerContext::SER_DATA_C_FILE:
        m_handle = reinterpret_cast<intptr_t>(m_context.fin);
        break;
    case CSerContext::SER_DATA_FD:
        m_handle = m_context.fd;
        break;
    case CSerContext::SER_DATA_FNAME:
        if (!ParallelSubSystemHelper::isInited())
            return;
        MPI_SAFE_CALL(MPI_File_open(MPI_COMM_WORLD, m_context.filename,
                                    MPI_MODE_RDWR,
                                    MPI_INFO_NULL,
                                    &m_MPIFile.getRef()));
        break;
    default:
        break;
    }
}


int xread(int fd, void *buffer, size_t size)
{
    int rs = 0;
    byte_t *pbuffer = (byte_t *)(buffer);
    while (size > 0 && (rs = read(fd, pbuffer, size)) >= 0) {
        size -= rs;
        pbuffer += rs;
    }
    if (size == 0)
        return 0;
    return rs;
}


int xwrite(int fd, const void *buffer, size_t size)
{
    int ws = 0;
    byte_t *pbuffer = (byte_t *)(buffer);
    while (size > 0 && (ws = write(fd, pbuffer, size)) >= 0) {
        size -= ws;
        pbuffer += ws;
    }
    if (size == 0)
        return 0;
    return ws;
}

result_t contextIO::readHeader(byteArray &out)
{
    m_rheaderSize = out.size();
    switch (m_context.getMode())
    {
    case CSerContext::SER_DATA_BUFFER:
        {
            if (out.size() > m_pSize) {
                return X_FAIL;
            }
            byte_t *pBegin = reinterpret_cast<byte_t *>(m_handle);
            std::copy(pBegin, pBegin + out.size(), out.begin());
            return X_SOK;
        }
        break;
    case CSerContext::SER_DATA_C_FILE:
        {
            if (std::fseek(m_context.fin, 0, SEEK_SET))
                return X_FAIL;
            size_t nres = std::fread(&out[0], out.size(), 1, m_context.fin);
            return (nres == out.size()) ? X_SOK : X_FAIL;
        }
        break;
    case CSerContext::SER_DATA_FD:
        {
            if (lseek(m_context.fd, 0, SEEK_SET))
                return X_FAIL;
            return (0 == xread(m_context.fd, &out[0], out.size())) ? X_SOK : X_FAIL;
        }
        break;
    case CSerContext::SER_DATA_FNAME:
        {
            if (!ParallelSubSystemHelper::isInited())
                return X_FAIL;
            int ret =  MPI_File_seek(m_MPIFile.getRef(), 0, SEEK_SET);
            if (MPI_SUCCESS != ret)
            {
                MPI_PRINT_ERROR(ret);
                return X_FAIL;
            }

            MPI_Status st;
            ret = MPI_File_read(m_MPIFile.getRef(), &out[0], out.size(), MPI_BYTE, &st);
            if (MPI_SUCCESS != ret || MPI_SUCCESS != st.MPI_ERROR)
            {
                MPI_PRINT_ERROR(ret);
                q_log_always_impl(st.MPI_ERROR, ParallelSubSystemHelper::getConfig().rank);
                q_log_always_impl(st.MPI_SOURCE, ParallelSubSystemHelper::getConfig().rank);
                q_log_always_impl(st.MPI_TAG, ParallelSubSystemHelper::getConfig().rank);
                return X_FAIL;
            }
            return X_SOK;
        }
        break;
    default:
        break;
    }
    return X_FAIL;
}


result_t contextIO::writeHeader(const byteArray &out)
{
    m_wheaderSize = out.size();
    switch (m_context.getMode())
    {
    case CSerContext::SER_DATA_BUFFER:
        {
            if (out.size() > m_pSize) {
                return X_FAIL;
            }
            byte_t *pBegin = reinterpret_cast<byte_t *>(m_handle);
            std::copy(out.begin(), out.end(), pBegin);
            return X_SOK;
        }
        break;
    case CSerContext::SER_DATA_C_FILE:
        {
            if (std::fseek(m_context.fin, 0, SEEK_SET))
                return X_FAIL;
            size_t nres = std::fwrite(&out[0], out.size(), 1, m_context.fin);
            return (nres == out.size()) ? X_SOK : X_FAIL;
        }
        break;
    case CSerContext::SER_DATA_FD:
        {
            if (lseek(m_context.fd, 0, SEEK_SET))
                return X_FAIL;
            return (0 == xwrite(m_context.fd, &out[0], out.size())) ? X_SOK : X_FAIL;
        }
        break;
    case CSerContext::SER_DATA_FNAME:
        {
            if (!ParallelSubSystemHelper::isInited())
                return X_FAIL;

            int ret =  MPI_File_seek(m_MPIFile.getRef(), 0, SEEK_SET);
            if (MPI_SUCCESS != ret)
            {
                MPI_PRINT_ERROR(ret);
                return X_FAIL;
            }

            MPI_Status st;
            ret = MPI_File_write(m_MPIFile.getRef(), &out[0], out.size(), MPI_BYTE, &st);
            if (MPI_SUCCESS != ret || MPI_SUCCESS != st.MPI_ERROR)
            {
                MPI_PRINT_ERROR(ret);
                q_log_always_impl(st.MPI_ERROR, ParallelSubSystemHelper::getConfig().rank);
                q_log_always_impl(st.MPI_SOURCE, ParallelSubSystemHelper::getConfig().rank);
                q_log_always_impl(st.MPI_TAG, ParallelSubSystemHelper::getConfig().rank);
                return X_FAIL;
            }
            return X_SOK;
        }
        break;
    default:
        break;
    }
    return X_FAIL;
}


result_t contextIO::read(byteArray &out, off64_t offset, uint64_t allSize)
{
    off64_t effOff = offset + m_rheaderSize;
    switch (m_context.getMode())
    {
    case CSerContext::SER_DATA_BUFFER:
        {
            if (effOff + out.size() > m_pSize) {
                return X_FAIL;
            }
            byte_t *pBegin = reinterpret_cast<byte_t *>(m_handle);
            std::copy(pBegin + effOff, pBegin + effOff + out.size(), out.begin());
            return X_SOK;
        }
        break;
    case CSerContext::SER_DATA_C_FILE:
        {
            if (std::fseek(m_context.fin, effOff, SEEK_SET))
                return X_FAIL;
            size_t nres = std::fread(&out[0], out.size(), 1, m_context.fin);
            return (nres == out.size()) ? X_SOK : X_FAIL;
        }
        break;
    case CSerContext::SER_DATA_FD:
        {
            if (lseek(m_context.fd, effOff, SEEK_SET))
                return X_FAIL;
            return (0 == xread(m_context.fd, &out[0], out.size())) ? X_SOK : X_FAIL;
        }
        break;
    case CSerContext::SER_DATA_FNAME:
        {
            if (!ParallelSubSystemHelper::isInited())
                return X_FAIL;

            ScopedMPIType dt;
            std::vector<int> bls(ParallelSubSystemHelper::getConfig().size);
            std::vector<int> disps(ParallelSubSystemHelper::getConfig().size);

            size_t basicBlockSize = out.size();
            size_t lastBlockSize = basicBlockSize;
            if (ParallelSubSystemHelper::getConfig().rank == ParallelSubSystemHelper::getConfig().size - 1)
            {
                if (ParallelSubSystemHelper::getConfig().rank == 1) {
                    basicBlockSize = offset;
                } else {
                    basicBlockSize = offset / (ParallelSubSystemHelper::getConfig().rank - 1);
                }
                lastBlockSize = allSize - (ParallelSubSystemHelper::getConfig().size - 1) * basicBlockSize;
            }

            for (int i = 0; i < ParallelSubSystemHelper::getConfig().size; ++i){
                bls[i] = (i < ParallelSubSystemHelper::getConfig().size - 1) ? basicBlockSize : lastBlockSize;
                disps[i] = i * basicBlockSize;
            }

            MPI_SAFE_CALL(MPI_Type_indexed(ParallelSubSystemHelper::getConfig().size, &bls[0], &disps[0], MPI_BYTE, &dt.getRef()));
            MPI_SAFE_CALL(MPI_File_set_view(m_MPIFile.getRef(), m_rheaderSize, MPI_BYTE, dt.getRef(), "native", NULL));

            MPI_Status st;
            int ret = MPI_File_read_all(m_MPIFile.getRef(), &out[0], out.size(), MPI_BYTE, &st);

            if (MPI_SUCCESS != ret || MPI_SUCCESS != st.MPI_ERROR)
            {
                MPI_PRINT_ERROR(ret);
                q_log_always_impl(st.MPI_ERROR, ParallelSubSystemHelper::getConfig().rank);
                q_log_always_impl(st.MPI_SOURCE, ParallelSubSystemHelper::getConfig().rank);
                q_log_always_impl(st.MPI_TAG, ParallelSubSystemHelper::getConfig().rank);
                return X_FAIL;
            }
            return X_SOK;

        }
        break;
    default:
        break;
    }
    return X_FAIL;
}

result_t contextIO::write(const byteArray &in, off64_t offset, uint64_t allSize)
{
    off64_t effOff = m_wheaderSize + offset;
    switch (m_context.getMode())
    {
    case CSerContext::SER_DATA_BUFFER:
        {
            if (effOff + in.size() > m_pSize) {
                return X_FAIL;
            }
            byte_t *pBegin = reinterpret_cast<byte_t *>(m_handle);
            std::copy(in.begin(), in.end(), pBegin + effOff);
            return X_SOK;
        }
        break;
    case CSerContext::SER_DATA_C_FILE:
        {
            if (std::fseek(m_context.fin, effOff, SEEK_SET))
                return X_FAIL;
            size_t nres = std::fwrite(&in[0], in.size(), 1, m_context.fin);
            return (nres == in.size()) ? X_SOK : X_FAIL;
        }
        break;
    case CSerContext::SER_DATA_FD:
        {
            if (lseek(m_context.fd, effOff, SEEK_SET))
                return X_FAIL;
            return (0 == xwrite(m_context.fd, &in[0], in.size())) ? X_SOK : X_FAIL;
        }
        break;
    case CSerContext::SER_DATA_FNAME:
        {
            if (!ParallelSubSystemHelper::isInited())
                return X_FAIL;

            ScopedMPIType dt;
            std::vector<int> bls(ParallelSubSystemHelper::getConfig().size);
            std::vector<int> disps(ParallelSubSystemHelper::getConfig().size);

            size_t basicBlockSize = in.size();
            size_t lastBlockSize = basicBlockSize;
            if (ParallelSubSystemHelper::getConfig().rank == ParallelSubSystemHelper::getConfig().size - 1)
            {
                if (ParallelSubSystemHelper::getConfig().rank == 1) {
                    basicBlockSize = offset;
                } else {
                    basicBlockSize = offset / (ParallelSubSystemHelper::getConfig().rank - 1);
                }
                lastBlockSize = allSize - (ParallelSubSystemHelper::getConfig().size - 1) * basicBlockSize;
            }

            for (int i = 0; i < ParallelSubSystemHelper::getConfig().size; ++i){
                bls[i] = (i < ParallelSubSystemHelper::getConfig().size - 1) ? basicBlockSize : lastBlockSize;
                disps[i] = i * basicBlockSize;
            }

            MPI_SAFE_CALL(MPI_Type_indexed(ParallelSubSystemHelper::getConfig().size, &bls[0], &disps[0], MPI_BYTE, &dt.getRef()));
            MPI_SAFE_CALL(MPI_File_set_view(m_MPIFile.getRef(), m_rheaderSize, MPI_BYTE, dt.getRef(), "native", NULL));

            MPI_Status st;
            int ret = MPI_File_write_all(m_MPIFile.getRef(), &in[0], in.size(), MPI_BYTE, &st);

            if (MPI_SUCCESS != ret || MPI_SUCCESS != st.MPI_ERROR)
            {
                MPI_PRINT_ERROR(ret);
                q_log_always_impl(st.MPI_ERROR, ParallelSubSystemHelper::getConfig().rank);
                q_log_always_impl(st.MPI_SOURCE, ParallelSubSystemHelper::getConfig().rank);
                q_log_always_impl(st.MPI_TAG, ParallelSubSystemHelper::getConfig().rank);
                return X_FAIL;
            }
        }


        break;
    default:
        break;
    }
    return X_FAIL;
}


