#include "qmatrix.h"

QMatrix::QMatrix(int w, int h)
{
    rows_cnt = h;
    columns_cnt = w;
    data_ptr = new mcomplex *[h];
    for (int i = 0; i < h; i++) {
        data_ptr[i] = new mcomplex[w];
    }
}

QMatrix::~QMatrix()
{
    for (int i = 0; i < rows_cnt; i++) {
        delete [] data_ptr[i];
    }
    delete [] data_ptr;
}
