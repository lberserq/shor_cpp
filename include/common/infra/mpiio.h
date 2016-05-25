#ifndef MPIIO_H
#define MPIIO_H
#include <common/config.h>
#include <common/utils.h>



namespace deleters
{
    template<>  inline void deleter<MPI_File>(MPI_File &handle)
    {
        MPI_SAFE_CALL(MPI_File_close(&handle));
    }

    template<>  inline void deleter<MPI_Datatype>(MPI_Datatype &handle)
    {
        MPI_SAFE_CALL(MPI_Type_free(&handle));
    }


}
typedef ScopedHandle<MPI_File> ScopedMPIFile;
typedef ScopedHandle<MPI_Datatype> ScopedMPIType;


#endif // MPIIO_H

