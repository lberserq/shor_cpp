#ifndef TESTS_H
#define TESTS_H

namespace tests {

    extern int width;

    void HadmardTest();
    void AdderTest();
    void MulTest();
    void NotTest();
    void CNOTTest();
    void ToffoliTest();
    void expTest();
    void QFTTest();
    void MeasureTest();
    void SwapTest();
    void collapseTest();
    void fake_test();

    void ShorTest();
    void MatrixTest();
    void CraussTest();

    void SerializeTest();
}

#endif // TESTS_H
