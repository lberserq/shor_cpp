#ifndef QSCRIPT_STUBS_H
#define QSCRIPT_STUBS_H
#include <common_impl.h>
#include <cvariant.h>
#include <qmath.h>
#include <qft.h>
#include <multiplier.h>
extern service_ptr_t<IQRegister> gRegister;
namespace QStubs
{
    template <class T> void VectorSize(size_t width) {
        gRegister = service_ptr_t<IQRegister>(new T(width, 0));
    }

    inline void PrintStub(const cVariant &x) {
        q_log(x.toString());
    }

    inline void Hadamard(int id1) {
        ::ApplyHadamard(*gRegister.get(), id1);
    }

    inline void SigmaX(int id1)  {
        ::ApplyNot(*gRegister.get(), id1);
    }


    inline void QBitOp(const QMatrix &m, int id1) {
        ::ApplyQbitMatrix(m, *gRegister.get(), id1);
    }


    inline void DiQBitOp(const QMatrix &m, int id1, int id2) {
        ::ApplyDiQbitMatrix(m, *gRegister.get(), id1, id2);
    }

    inline void TriQBitOp(const QMatrix &m, int id1, int id2, int id3) {
        ::ApplyTriQbitMatrix(m, *gRegister.get(), id1, id2, id3);
    }

    inline void CNOT(int id1, int id2) {
        ::ApplyCnot(*gRegister.get(), id1, id2);
    }

    inline void Toffolli(int id1, int id2, int id3) {
        ::ApplyToffoli(*gRegister.get(),  id1, id2, id3);
    }


    inline QMatrix DeclareMatrix4(const std::vector<cVariant> &tuple) {
        std::vector<complex_t> tupleVal;
        for (size_t i = 0; i < tuple.size(); ++i) {
            tupleVal.push_back(tuple[i].get<complex_t>());
        }

        const int dim = 2;

        QMatrix m(dim, dim);
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                m(i, j) = tupleVal[i * dim + j];
            }
        }
        return m;
    }



    inline QMatrix DeclareMatrix16(const std::vector<cVariant> &tuple) {
        std::vector<complex_t> tupleVal;
        for (size_t i = 0; i < tuple.size(); ++i) {
            tupleVal.push_back(tuple[i].get<complex_t>());
        }
        const int dim = 4;

        QMatrix m(dim, dim);
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                m(i, j) = tupleVal[i * dim + j];
            }
        }
        return m;
    }



    inline QMatrix DeclareMatrix64(const std::vector<cVariant> &tuple) {
        std::vector<complex_t> tupleVal;
        for (size_t i = 0; i < tuple.size(); ++i) {
            tupleVal.push_back(tuple[i].get<complex_t>());
        }
        const int dim = 8;

        QMatrix m(dim, dim);
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                m(i, j) = tupleVal[i * dim + j];
            }
        }
        return m;
    }



    inline void SigmaY(int id1) {
        QMatrix m(2, 2);
        m(0,0)=m(1, 1) = 0;
        m(0, 1) = -QMath::i;
        m(1, 0) = QMath::i;
        QBitOp(m, id1);
    }



    inline void SigmaZ(int id1) {
        QMatrix m(2, 2);
        m(0, 0) = m(1, 1) = 0;
        m(0, 1) = -QMath::i;
        m(1, 0) = QMath::i;
        QBitOp(m, id1);
    }

    inline void QBitOp(int id1) {
        QMatrix m(2, 2);
        m(0, 0) = 1;
        m(1, 1) = -1;
        m(1, 0) = m(0, 1) = 0;
        QBitOp(m, id1);
    }


    inline void Swap(int id1, int id2) {
        ::ApplySWAP(*gRegister.get(), id1, id2);
    }

    inline void QFT(int start, int width) {
        std::vector<int> idx;
        for (size_t i = 0; i < static_cast<size_t>(width); ++i) {
            idx.push_back(start + i);
        }
        QFT::ApplyQFT(*gRegister.get(), width, idx);
    }

    inline void InvQFT(int start, int width) {
        std::vector<int> idx;
        for (size_t i = 0; i < static_cast<size_t>(width); ++i) {
            idx.push_back(start + i);
        }
        QFT::ApplyQFTInv(*gRegister.get(), width, idx);
    }

    inline void MeasureBit(int i) {
        QRegHelpers::DeleteVar(*gRegister.get(), i);
    }

    inline void ExpModN(int x, int N, int width) {
        ::expamodn(*gRegister.get(), N, x, gRegister->getWidth(), width);
    }




}

#endif // QSCRIPT_STUBS_H

