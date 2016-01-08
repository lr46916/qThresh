
namespace memoryUtils {
    //method used to allocate 2D arrays for data storage.
    //total memory usage is O(f(m,k)) as defined in the paper.
    //@param rowSize size of each row
    //@param k number of rows
    int** mallocIntArrays(int* rowSize, int k);
    
    //Method used to delete any kind of 2D data.
    //@param data Memory to be freed
    //@param k number of rows
    template<typename T> void free2DArray(T **data, int k) {
        for(int i = 0; i < k; i++) delete data[i];
        delete data;
    }

    //This method is used to allocate data storage for all neighbourhood masks
    //used in this algorithm. Values are stored in array so reading would be 
    //as fast as possible.
    //@param size size of neighbourhood data
    int** mallocNeighbourhoodData(int size); 

    //Method that allocates memory that will be used to store optimal subsolutions while
    //running DP recurrence. Each thread needs 2 of such storage (one for "last" subsolution
    //calculated and one for subsolution/solution that is currently being computed in DP
    //recurrence).
    //@param k value indicating value of missmatches used in DP recurrence algorithm
    //@param maxBinCoefPrefSum array of values containing maximum binom coefficient values
    //                         (ones that were computed using largest span value). With this
    //                         it is guarateed that for any span value we have enough memory
    //                         at our disposal.
    //@param numberOfThreads number of threads that will be used to compute threshold values
    //                       concurrently
    int*** preallocateDPArrays(int k, int* maxBinCoefPrefSum, int numberOfThreads); 
}
