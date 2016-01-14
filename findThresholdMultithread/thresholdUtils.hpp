namespace thresholdUtils {
    typedef unsigned long long ullong;
    
    //Method used to compute all bit masks that are to be used in computation.
    //Returns a single array that contains all M sets that will need to be processed in
    //optimal threshold computation. Sets are sorted ascendingly by the number of missmatch.
    //Each element (bit) missing from the set is a missmatch.
    //@param binCoefCount zero-indexed array with computed binom coefficient values of size k+1. 
    //       Each index indicates a value (s-1) choose i, where i is an index.
    //@param k maximal number of missmatches
    //@param s span of a shape for which this bit-mask array is being computed for
    //@returns computed array of bit masks. Total size of sum(binomCoefCount)
    ullong* computeBitmaskArray(int* binomCoefCount, int k, int s);

    //Method used to calculate all (s-1) choose i values needed for the
    //computation. i ranges from 0 up to the k (included).
    //@param k number of missmatches
    //@param s span size
    //@returns an array of all computed binom coefficients. resultArray[i] = (s-1) choose i
    int* calculateBinomCoef(int k, int s);     
}
