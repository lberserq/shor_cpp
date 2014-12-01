#ifndef QMATRIX_H
#define QMATRIX_H
#include "config.h"
class QMatrix
{
    mcomplex ** data_ptr;
    int rows_cnt;
    int columns_cnt;
public:
    QMatrix(int w, int h);
    mcomplex & operator() (int i, int j) {
        return (data_ptr[i][j]);
    }
    const
    mcomplex & operator() (int i, int j) const {
        return (data_ptr[i][j]);
    }
   /* mcomplex operator() (int i, int j) {
        return (data_ptr[i][j]);
    }*/
    ~QMatrix();
};


#endif // QMATRIX_H
