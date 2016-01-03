#include <stdlib.h>
#include "thresholdUtils.hpp"

namespace thresholdUtils {

    //Method used to compute all bit masks that are to be used in computation.
    //Returns a single array that contains all M sets that will need to be processed in
    //optimal threshold computation. Sets are sorted ascendingly by the number of missmatch.
    //Each element (bit) missing from the set is a missmatch.
    //@param binCoefCount zero-indexed array with computed binom coefficient values of size k+1. 
    //       Each index indicates a value (s-1) choose i, where i is an index.
    //@param k maximal number of missmatches
    //@param s span of a shape for which this bit-mask array is being computed for
    //@returns computed array of bit masks. Total size of sum(binomCoefCount)
    ullong* computeBitmaskArray(int* binomCoefCount, int k, int s) {
        
        ullong* result = NULL;
        
        ullong resultMask = (1LL << s) - 1;

        int total = 0;
        for(int i = 0; i <= k; i++) {
            total += binomCoefCount[i];
        }

        result = (ullong*) malloc(sizeof(ullong) * total);
        
        //in this loop we compute all masks. First we compute all sets
        //that have 0 missmatch, then 1 missmatch... and so on, up to k
        int c = 0;
        for(int i = 0; i <= k; i++) {
            
            ullong current = (1LL << i) - 1;
            //current = current | (current - 1);

            for(int j = 0; j < binomCoefCount[i]; j++) {
                //we shift the result by one because sets contain elements [1,s-1], therefore
                //we need to shift resut by one to the left in order to have 0-th element not set.
                result[c++] = ((~current) << 1) & resultMask;
                ullong tmp = current | (current - 1); 
                current = (tmp + 1) | (((~tmp & -~tmp) - 1)) >> (__builtin_ctzll(current) + 1);
            } 
        
        }
        
        return result;
    }

    //method used to calculate all (s-1) choose i values needed for the
    //computation. i ranges from 0 up to the k (included).
    //@returns an array of all computed binom coefficients. resultArray[i] = (s-1) choose i
    int* calculateBinomCoef(int k, int s) {
        
        int* result = (int*) malloc(sizeof(int) * (k+1));
        
        result[0] = 1;
        result[1] = s-1;

        int top = s-1;
        int bottom = 1;
       
        for(int i = 2; i <= k; i++) {
            top *= s - i;
            bottom *= i;
            result[i] = top / bottom;
        } 
        
        return result;
    }
}
