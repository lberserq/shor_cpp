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
    template <class T> void VectorSize(size_t width, representation_t mode = REG_REPRESENTATION) {
        gRegister = service_ptr_t<IQRegister>(new T(width, 0, 1.0, mode));
        q_log("LocalSize");
        q_log(gRegister->getStatesSize());
        q_log("ALLSIZE");
        int sz = ParallelSubSystemHelper::isInited() ? ParallelSubSystemHelper::getConfig().size : 1;
        q_log((gRegister->getStatesSize() * sz));
    }



    inline void PrintStub(const cVariant &x)
    {
        if (ParallelSubSystemHelper::isInited()) {
            q_log(x.toString());
        } else {
            std::cerr << x.toString() << std::endl;
        }
    }

    inline void Hadamard(const cVariant &id1) {
        ::ApplyHadamard(*gRegister.get(), id1.get<int_t>());
    }

    inline void SigmaX(const cVariant &id1)  {
        ::ApplyNot(*gRegister.get(), id1.get<int_t>());
    }


    inline void QBitOp(const QMatrix &m, const cVariant &id1) {
        ::ApplyQbitMatrix(m, *gRegister.get(), id1.get<int_t>());
    }


    inline void DiQBitOp(const QMatrix &m, const cVariant &id1, const cVariant &id2) {
        ::ApplyDiQbitMatrix(m, *gRegister.get(), id1.get<int_t>(), id2.get<int_t>());
    }

    inline void TriQBitOp(const QMatrix &m, const cVariant &id1, const cVariant &id2, const cVariant &id3) {
        ::ApplyTriQbitMatrix(m, *gRegister.get(), id1.get<int_t>(), id2.get<int_t>(), id3.get<int_t>());
    }

    inline void CNOT(const cVariant &id1, const cVariant &id2) {
        ::ApplyCnot(*gRegister.get(), id1.get<int_t>(), id2.get<int_t>());
    }

    inline void Toffolli(const cVariant &id1, const cVariant &id2, const cVariant &id3) {
        ::ApplyToffoli(*gRegister.get(),  id1.get<int_t>(), id2.get<int_t>(), id3.get<int_t>());
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



    inline void SigmaY(const cVariant &id1) {
        QMatrix m(2, 2);
        m(0,0)=m(1, 1) = 0;
        m(0, 1) = -QMath::i;
        m(1, 0) = QMath::i;
        QBitOp(m, id1);
    }



    inline void SigmaZ(const cVariant &id1) {
        QMatrix m(2, 2);
        m(0, 0) = m(1, 1) = 0;
        m(0, 1) = -QMath::i;
        m(1, 0) = QMath::i;
        QBitOp(m, id1);
    }

    inline void QBitOp(const cVariant &id1) {
        QMatrix m(2, 2);
        m(0, 0) = 1;
        m(1, 1) = -1;
        m(1, 0) = m(0, 1) = 0;
        QBitOp(m, id1);
    }


    inline void Swap(const cVariant &id1, const cVariant &id2) {
        ::ApplySWAP(*gRegister.get(), id1.get<int_t>(), id2.get<int_t>());
    }

    inline void QFT(const cVariant &start, const cVariant &width) {
        std::vector<int> idx;
        for (size_t i = 0; i < static_cast<size_t>(width); ++i) {
            idx.push_back(start.get<int_t>() + i);
        }
        ::QFT::ApplyQFT(*gRegister.get(), width.get<int_t>(), idx);
    }

    inline void InvQFT(const cVariant &start, const cVariant &width) {
        std::vector<int> idx;
        for (size_t i = 0; i < static_cast<size_t>(width); ++i) {
            idx.push_back(start.get<int_t>() + i);
        }
        ::QFT::ApplyQFTInv(*gRegister.get(), width.get<int_t>(), idx);
    }

    inline void MeasureBit(const cVariant &i) {
        QRegHelpers::DeleteVar(*gRegister.get(), i.get<int_t>());
    }

    namespace
    {
        int get_dim(int_t n) {
            int_t i = 0;
            while (static_cast<int_t>(1<< i) < n) {
                i++;
            }
            return i;
        }
    }

    inline void ExpModN(const cVariant &x, const cVariant &N, const cVariant &width) {
        int_t Ni = N.get<int_t>();
        int_t all_width = get_dim(Ni  * Ni);
        int_t width_local = get_dim(Ni);
        int_t local_variables_size = 3 * width_local + 2;
        int_t currentWidth = gRegister.get()->getWidth();
        if (currentWidth < local_variables_size + all_width)
            gRegister->allocSharedMem(local_variables_size + all_width - currentWidth);

        q_log("LocalSize");
        q_log(gRegister->getStatesSize());
        q_log("ALLSIZE");
        int sz = ParallelSubSystemHelper::isInited() ? ParallelSubSystemHelper::getConfig().size : 1;
        q_log(gRegister->getStatesSize() * sz);

        //expamodn(*tmp, n, a, all_width, width_local);
        if (width.get<int_t>() > local_variables_size) {
            width_local = width.get<int_t>() - (local_variables_size - width_local);
        }

        ::expamodn(*gRegister.get(), N.get<int_t>(), x.get<int_t>(), all_width, width_local);
        QRegHelpers::DeleteLocalVars(*gRegister.get(), local_variables_size);
    }

    inline void ShiftLeft(const cVariant &x) {
        int_t x_i = x.get<int_t>();
        for (int_t i = 0; i < x_i; i++) {
            Swap(x_i + i, i);
        }
    }

    inline void ShiftRight(const cVariant &) {
    }

    inline void PrintReg() {
        gRegister->print();
    }
}
namespace QMath {
    inline std::vector<cVariant> fracApprox(uint_type c, uint_type q,  uint_type width)
    {
        std::vector<uint_type> res = conFracApprox(c, q, width);
        std::vector<cVariant> resV;
        resV.push_back(res[0]);
        resV.push_back(res[1]);
        return resV;
    }

}


#endif // QSCRIPT_STUBS_H

