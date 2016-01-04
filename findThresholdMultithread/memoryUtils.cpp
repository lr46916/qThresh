#include <stdlib.h>
#include "memoryUtils.hpp"

namespace memoryUtils {
    
    int** mallocIntArrays(int* rowSize, int k){
        
        int** result = (int**) malloc(sizeof(int*) * k);

        for(int i = 0; i < k; i++) {
            result[i] = (int*) malloc(sizeof(int) * rowSize[i]);
        }
        
        return result;
    }

    int** mallocNeighbourhoodData(int size) {
        int **result = (int**) malloc(sizeof(int*) * size);

        for(int i = 0; i < size; i++) {
            result[i] = (int*) malloc(sizeof(int) * 2);
        }

        return result;
    }

    int*** preallocateDataArraysForDP(int k, int* maxBinCoefPrefSum, int numberOfThreads) {
        
        int*** result = (int***) malloc(sizeof(int**) * 2 * numberOfThreads);

        for(int i = 0; i < numberOfThreads * 2; i++) {
            result[i] = mallocIntArrays(maxBinCoefPrefSum, k+1); 
        }

        return result;
    }
}
