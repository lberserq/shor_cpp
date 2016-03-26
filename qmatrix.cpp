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

QMatrix operator + (const QMatrix &left, const QMatrix &right) {
    QMatrix res(left.rows_cnt, left.columns_cnt);
    for (int i = 0; i < left.rows_cnt; i++) {
        for (int j = 0; j < left.columns_cnt; j++) {
            res.data_ptr[i][j] = left.data_ptr[i][j] + right.data_ptr[i][j];
        }
    }
    return res;
}


QMatrix operator * (const QMatrix &left, const QMatrix &right) {
    QMatrix res(left.rows_cnt, right.columns_cnt);
    for (int i = 0; i < left.rows_cnt; i++) {
        for (int j = 0; j < right.columns_cnt; j++) {
            for (int k = 0; k < right.rows_cnt; k++) {
                res.data_ptr[i][j] = left.data_ptr[i][k] * right.data_ptr[k][j];
            }
        }
    }
    return res;
}
