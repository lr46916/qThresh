
namespace memoryUtils {
    int** mallocIntArrays(int* rowSize, int k);
    template<typename T> void free2DArray(T **data, int k) {
        for(int i = 0; i < k; i++) {
            free(data[i]);
        }
        free(data);
    }
    int** mallocNeighbourhoodData(int size); 
    int*** preallocateDataArraysForDP(int k, int* maxBinCoefPrefSum, int numberOfThreads); 
}
