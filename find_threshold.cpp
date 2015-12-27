#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#define min(x,y) x < y ? x : y

using namespace std;

typedef unsigned long long ullong;

//method used to compute all bit masks that are to be used in computation.
//Returns a single array that contains all M sets that will need to be processed in
//optimal threshold computation. Sets are sorted ascendingly by the number of missmatch.
//Each element (bit) missing from the set is a missmatch.
ullong* computeBitmaskArray(int* binomCoefCount, int k, int s) {
    
    ullong* result = NULL;
    
    ullong resultMask = (1LL << s) - 1;

    int total = 0;
    for(int i = 0; i < k; i++) {
        total += binomCoefCount[i];
    }

    result = (ullong*) malloc(sizeof(ullong) * total);
    
    ullong bot32Mask = (1LL << 32) - 1;
    
    //in this loop we compute all masks. First we compute all sets
    //that have 0 missmatch, then 1 missmatch... and so on, up to k
    int c = 0;
    for(int i = 0; i < k; i++) {
        
        ullong current = (1LL << i) - 1;
        //current = current | (current - 1);

        for(int j = 0; j < binomCoefCount[i]; j++) {
            //we shift the result by one because sets contain elements [1,s-1], therefore
            //we need to shift resut by one to the left in order to have 0-th element not set.
            result[c++] = ((~current) << 1) & resultMask;
            ullong tmp = current | (current - 1); 
            int shift = 0;
            uint input = (uint) current;
            if ((bot32Mask & current) == 0) { 
                input = (uint) (current >> 32);
                shift = 32;
            }
            shift += __builtin_ctz(input) + 1;
            
            current = (tmp + 1) | (((~tmp & -~tmp) - 1)) >> shift;
        } 
    
    }
    
    return result;
}

//method used to calculate all (s-1) choose k values needed for the
//computation.
int* calculateBinomCoef(int k, int s) {
    
    int* result = (int*) malloc(sizeof(int) * k);
    
    result[0] = 1;
    result[1] = s-1;

    int top = s-1;
    int bottom = 1;
   
    for(int i = 2; i < k; i++) {
        top *= s - i;
        bottom *= i;
        result[i] = top / bottom;
    } 
    
    return result;
}

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
void freeIntArrays(int**data, int k) {
    for(int i = 0; i < k; i++) {
        free(data[i]);
    }
}

//fill all elements of 2D array with given value 
//@param data 2D array to fill
void fill(int** data, int* rowSize, int k, int value) {
    for(int i = 0; i < k; i++) {
        for(int j = 0; j < rowSize[i]; j++) {
            data[i][j] = value;
        }
    }
}

void printArray(int* data, int size) {
    for(int i = 0; i < size; i++) {
        printf("%d, ", data[i]);
    }
    printf("size: %d\n", size);
}

void print2D(int** data, int* rowSize, int k) {
    for(int i = 0; i < k; i++) {
        printArray(data[i], rowSize[i]);
    }
}

int findThreshold(int* shape, int size, int m, int k) {
    
    if (m > 64 || size > m || k > size) {
        fprintf(stderr,"Invalid input arguments. shape size: %d, m: %d, k: %d\n", size, m, k);
        exit(-1);
    }

    //to include k
    k++;

    int maxInt = 2147483647;

    //mask used check whether some other set M is a subset of shape.
    ullong shape_mask = 0;

    int min = shape[0];
    int max = 0;

    for(int i = 0; i < size; i++) {
        shape_mask |= (1LL << shape[i]);
        if (shape[i] > max) {
            max = shape[i];
        } else {
            if(shape[i] < min) {
                min = shape[i];
            }
        }
    }

    int span = max - min + 1;
   
    ullong lastElemMask = 1LL << (span - 1);

    int* binCoef = calculateBinomCoef(k, span);

    int binCoefPrefSum[k];

    binCoefPrefSum[0] = binCoef[0];

    for(int i = 1; i < k; i++) {
        binCoefPrefSum[i] = binCoef[i] + binCoefPrefSum[i-1];
    }

    int arraySize = binCoefPrefSum[k-1];

    ullong* bitMaskArray = computeBitmaskArray(binCoef, k, span); 
    
    unordered_map<ullong, int> maskToIndex;

    for(int i = 0; i < arraySize; i++) {
        maskToIndex[bitMaskArray[i]] = i;
    }
    
    int **currentResult, **nextResult;
    currentResult = mallocIntArrays(binCoefPrefSum, k); 
    nextResult = mallocIntArrays(binCoefPrefSum, k);
    
    fill(currentResult, binCoefPrefSum, k, 0);

    ullong lastElemMaskNot = ~lastElemMask;

    for(int i = span; i <= m; i++) {

        //here we go through all values j and M to compute next iteration    
        for(int j = 0; j < k; j++) {
            //end is the index of last set that has at most j missmatches (all the sets that have at most j bits set to zero)
            int end = binCoefPrefSum[j];
    
            for(int l = 0; l < end; l++) {
                ullong mask = bitMaskArray[l];
                int targetJ = j;
                if ((mask & lastElemMask) == 0) {
                    targetJ--;
                }
                if(targetJ == -1) {
                    //TODO can be removed this will never happen
                    continue;
                }
                //mask in bottom line in paper recurrence
                ullong targetMask = (mask & lastElemMaskNot) << 1;
                //mask in top line in paper recurrence
                ullong targetMask2 = targetMask | 2LL;
               
                int maskInd = -1, maskInd2 = -1;

                auto it = maskToIndex.find(targetMask);

                if(it != maskToIndex.end()) {
                    maskInd = it->second;                
                }

                auto it2 = maskToIndex.find(targetMask2);

                if(it2 != maskToIndex.end()) {
                    maskInd2 = it2->second;
                }

                int hit = ((shape_mask & (mask | 1LL)) != 0 && (mask | 1LL) >= shape_mask) ? 1 : 0;
                
                int nextVal = maxInt;
                
                //if target mask has more then targetJ missmatches it is invalid
                if(maskInd != -1 && maskInd < binCoefPrefSum[targetJ]){
                    nextVal = currentResult[targetJ][maskInd];
                }

                if(maskInd2 != -1 && maskInd2 < binCoefPrefSum[targetJ]) {
                    int val = currentResult[targetJ][maskInd2] + hit;
                    if(val < nextVal)
                        nextVal = val;
                }

                if(nextVal != maxInt){
                    //printf("update with hit %d\n", hit);    
                    nextResult[j][l] = nextVal;// + hit;   
                } else {
                    //TODO just for debugging. This case should never be triggered
                    printf("ERROR\n");
                    exit(-1);
                } 
            }

        }

        int** tmp = currentResult;
        currentResult = nextResult;
        nextResult = tmp;
    }
   
    int result = currentResult[k-1][0];

    for(int i = 1; i < arraySize; i++) {
        if(currentResult[k-1][i] < result) {
            result = currentResult[k-1][i];
        }
    }

    freeIntArrays(currentResult, k);
    freeIntArrays(nextResult, k);
    free(bitMaskArray); 
    free(binCoef);

    return result;
}

int main() {
    
    // int shape[12] = {0,2,4,8,14,16,18,22,28,30,32,36};
    //
    // printf("result: %d\n", findThreshold(shape, 12, 50, 5));

    int shape[3] = {0,1,2};

    printf("result: %d\n", findThreshold(shape, 3, 8, 1));
    return 0;
}
