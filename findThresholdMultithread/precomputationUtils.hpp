
namespace precomputationUtils {
    typedef unsigned long long ullong;
    
    //This method is used to compute binom coefficient arrays for each
    //value of span ranging from 2 up to span. Results are computed for
    //fixed value of k.
    //@param span value of upper limit of span for which binom coefficient
    //            data arrays will be computed. Binom coefficient arrays
    //            are computed for SPAN values {2,3,4...,span}.
    //@param k number of missmatches.
    //@returns 2D array containing binom coefficient data arrays. (of total size = span - 1)
    int** precomputeBinCoefData(int span, int k);
    
    //This method simply computes all prefix sums of given binon coefficeint arrays
    //for all cases with span ranging from 2 to value of parameter span and fixed value
    //of k (given in parameter k).
    //@param binCoefData 2D array containing precomputed binom coefficient data arrays
    //       for all spans ranging from 2 to value of parameter span
    //@param span max value of span for which data is computed.
    //@param k number of missmatches.
    //@returns 2D array containing binom cefficient prefix sum arrays. (of total size = span - 1)
    int** precomuteBinCoefPrefSumData(int span, int k, int** binCoefData);

    //Method that precomputes all bit-mask arrays for span ranging from 2 up 
    //to value of parameter span with fixed value of k (given in parameter k).
    //@param span max span value for which data is precomputed.
    //@param k number of missmatches.
    //@param binCoefData 2D array containing precomputed binom coefficient data arrays
    //       for all spans ranging from 2 to value of parameter span
    //@returns 2D array containing bit-array mask data arrays. (of total size = span - 1) 
    ullong** precomputeSpanBitMaskArrays(int span, int k, int** binCoefData);

    //As defined in dinamic programming recurrence for each given bit-mask (shape) there will be only
    //two options of shapes ("neighbours") that determain sub-solutions from which we can compute next optimal threshold
    //value.
    //In this method we go through all bit-mask arrays (for span = 2 up to value fiven in parameter span)
    //and for each bit-mask (shape) we compute it's two target "neighbours". We use a std map implementation to define inverse
    //mapping from given shape (bit-mask) to its index in corresponding bit-mask array. Now for both computed
    //shapes (bit-masks) we can read theirs indexes in corresponding bit-mask array and store that index values
    //in our result data field.
    //Using this precomputed data we can, one that we start running DP recurrence, for each bit-mask (shape) index
    //can simply read its ("neighbours") indexes with a simple look-up (without need of using a map implementation
    //to get an index from a shape(bit-mask) and finally gain more speed in DP recurrence).
    //@param span max value of span for which data is computed.
    //@param k number of missmatches.
    //@param bitMaskArray Array containing bit-mask arrays computed for k and all SPAN values {2,3,4...,span}.
    //@param binCoefPrefSum Array containing binom coefficient arrays prefix sums computed for k and 
    //                      all SPAN values {2,3,4...,span}.
    //@returns 3D array. For each span value from 2 up to value defined with parameter span mask neighbourhood
    //         data (which is 2D array) is precomputed and all are returned as a result.
    int*** precomputeMaskNeighbourhoodMap(int span, int k, ullong** bitMaskArray, int** binCoefPrefSum);
}
