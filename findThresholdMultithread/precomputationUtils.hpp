
namespace precomputationUtils {
    typedef unsigned long long ullong;
    int** precomputeBinCoefData(int span, int k);
    int** precomuteBinCoefPrefSumData(int span, int k, int** binCoefData);
    ullong** precomputeSpanBitMaskArrays(int span, int k, int** binCoefData);
    int*** precomputeMaskNeighbourhoodMap(int span, int k, ullong** bitMaskArray, int** binCoefPrefSum);
}
