//Method for calculating optimal threshold for given shape, and values of m and k. DP recurrence is as defined in paper.
//@param shapeMask a shape for which optmial threshold is to be computed by this method
//@param span span of a given shape
//@param m size of pattern
//@param k number of missmatches
//@param bitMaskArray precomputed bit-mask array given to this method to increse speed of threshold computation
//@param binCoefPrefSum precomputed prefix sum array of an binom coefficinet values array needed for the computation.
//                      binom coefficient values array is defined as binCoefValArray[i] = (span-1) choose i, with i
//                      ranging from 0 up to k (included). Also precomputed to speed up this method call.
//@param currentResult preallocated memory in which last calculated optimal subsolution values are stored. (preallocated for performace gains)
//@param nextResult preallocated memory in which next itaration optimal subsolution/solution values are stored. (preallocated for performance gains)
int findThreshold(unsigned long long shapeMask, int span, int m, int k, int** maskNgs, unsigned long long *bitMaskArray, int* binCoefPrefSum, int** currentResult, int** nextResult);
