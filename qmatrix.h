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
    QMatrix(const QMatrix &tmp){
       rows_cnt = tmp.rows_cnt;
       columns_cnt= tmp.columns_cnt;
       data_ptr = new mcomplex *[tmp.rows_cnt];
       for (int i = 0; i < tmp.rows_cnt; i++) {
    		data_ptr[i] = new mcomplex[tmp.columns_cnt];
       }
       for (int i = 0; i < rows_cnt;i++) {
           for (int j = 0; j < columns_cnt; j++)  {
               data_ptr[i][j] = tmp.data_ptr[i][j];
           }
       }
    }

    QMatrix& operator =(const QMatrix &tmp) {
        for (int i = 0; i < rows_cnt; i++) {
            delete [] data_ptr[i];
        }
        delete [] data_ptr;
        data_ptr = new mcomplex*[tmp.rows_cnt];
        for (int i = 0; i < tmp.columns_cnt; i++) {
            data_ptr[i] = new mcomplex[tmp.columns_cnt];
        }
        for (int i = 0; i < rows_cnt;i++) {
            for (int j = 0; j < columns_cnt; j++)  {
                data_ptr[i][j] = tmp.data_ptr[i][j];
            }
        }
        return *this;
    }

    mcomplex & operator() (int i, int j) {
        return (data_ptr[i][j]);
    }

    const
    mcomplex & operator() (int i, int j) const {
        return (data_ptr[i][j]);
    }
   /* mcomplex operator() (int i, int j) {
        return (data_ptr[i][j]);a
    }*/

    QMatrix & operator *(const mcomplex &val) {
        for (int i = 0; i < rows_cnt; i++) {
            for (int j = 0; j < columns_cnt; j++) {
                data_ptr[i][j] *= val;
            }
        }
        return *this;
    }

    QMatrix operator *(const QMatrix &mat) {
        QMatrix res(rows_cnt, mat.columns_cnt);
        for (int i = 0; i < rows_cnt; i++) {
            for (int k = 0; k < mat.columns_cnt; k++) {
                mcomplex sum = 0.0f;
                for (int j = 0; j < columns_cnt; j++) {
                    sum += data_ptr[i][j] * mat.data_ptr[j][k];
                }
                res.data_ptr[i][k] = sum;
            }
        }
        return res;
    }

//    const QMatrix & operator *(const mcomplex &val) {
//        for (int i = 0; i < rows_cnt; i++) {
//            for (int j = 0; j < columns_cnt; j++) {
//                data_ptr[i][j] *= val;
//            }
//        }
//        return *this;
//    }

    ~QMatrix();
};


#endif // QMATRIX_H
