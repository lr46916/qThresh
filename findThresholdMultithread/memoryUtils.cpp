#include <stdlib.h>
#include "memoryUtils.hpp"

namespace memoryUtils {
    //method used to allocate 2D arrays for data storage.
    //total memory usage is O(f(m,k)) as defined in the paper.
    int** mallocIntArrays(int* rowSize, int k){
        
        int** result = (int**) malloc(sizeof(int*) * k);

        for(int i = 0; i < k; i++) {
            result[i] = (int*) malloc(sizeof(int) * rowSize[i]);
        }
        
        return result;
    }

    //free...
    // template<typename T> void free2DArray(T **data, int k) {
    //     for(int i = 0; i < k; i++) {
    //         free(data[i]);
    //     }
    //     free(data);
    // }

    //This method is used to allocate data storage for all neighbourhood masks
    //used in this algorithm. Values are stored in array so reading would be 
    //as fast as possible.
    int** mallocNeighbourhoodData(int size) {
        int **result = (int**) malloc(sizeof(int*) * size);

        for(int i = 0; i < size; i++) {
            result[i] = (int*) malloc(sizeof(int) * 2);
        }

        return result;
    }

    //Method that allocates memory that will be used to store optimal subsolutions while
    //running DP recurrence. Each thread needs 2 of such storage (one for "last" subsolution
    //calculated and one for subsolution/solution that is currently being computed in DP
    //recurrence).
    int*** preallocateDataArraysForDP(int k, int* maxBinCoefPrefSum, int numberOfThreads) {
        
        int*** result = (int***) malloc(sizeof(int**) * 2 * numberOfThreads);

        for(int i = 0; i < numberOfThreads * 2; i++) {
            result[i] = mallocIntArrays(maxBinCoefPrefSum, k+1); 
        }

        return result;
    }
}
