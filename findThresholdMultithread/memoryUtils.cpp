#include <stdlib.h>
#include "memoryUtils.hpp"

namespace memoryUtils {
    
    int** mallocIntArrays(int* rowSize, int k){
        int** result = new int*[k];
        for(int i = 0; i < k; i++) result[i] = new int[rowSize[i]];
        return result;
    }

    int** mallocNeighbourhoodData(int size) {
        int** result = new int*[size];
	for(int i = 0; i < size; i++) result[i] = new int[size * 2];
        return result;
    }

    // capacity -- the maximum possible prefix sum of binomial coefficients
    int*** preallocateDPArrays(int k, int* capacity, int nThreads) {
        int*** result = new int**[2 * nThreads];
        for(int i = 0; i < nThreads * 2; i++) result[i] = mallocIntArrays(capacity, k+1); 
        return result;
    }
}
