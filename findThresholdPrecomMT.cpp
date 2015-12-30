#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <list>
#include <unordered_set>
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <tuple>
#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <time.h>
#include <ctime>

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
            current = (tmp + 1) | (((~tmp & -~tmp) - 1)) >> (__builtin_ctzl(current) + 1);
        } 
    
    }
    
    return result;
}

//method used to calculate all (s-1) choose k values needed for the
//computation.
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
    free(data);
}

int** mallocNeighbourhoodData(int size) {
    int **result = (int**) malloc(sizeof(int*) * size);

    for(int i = 0; i < size; i++) {
        result[i] = (int*) malloc(sizeof(int) * 2);
    }

    return result;
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


ullong** precomputeSpanBitMaskArrays(int span, int k) {

    ullong** result = (ullong**) malloc((span - 1) * sizeof(ullong*));

    for(int i = 0; i < span - 1; i++) {
        int targetSpan = i + 2;

        int* binCoef = calculateBinomCoef(k, targetSpan);

        result[i] = computeBitmaskArray(binCoef, k, targetSpan); 
       
        free(binCoef);
    }

    return result;
}

int*** precomputeMaskNeighbourhoodMap(int span, int k, ullong** bitMaskArray) {
    
    int*** result = (int***) malloc(sizeof(int**) * (span-1));

    
    for(int i = 0; i < span - 1; i++) {
        int targetSpan = i + 2;
        int* binCoef = calculateBinomCoef(k, targetSpan);
        int arraySize = 0;
        for(int j = 0; j <= k; j++) {
            arraySize += binCoef[j];
        }
        free(binCoef);
        
        printf("computing for span %d...\n", targetSpan);
        
        unordered_map<ullong, int> maskToIndex;

        for(int j = 0; j < arraySize; j++) {
            maskToIndex[bitMaskArray[i][j]] = j;
        }

        ullong lastElemMask = 1LL << (targetSpan - 1);
        ullong lastElemMaskNot = ~lastElemMask;

        ullong errorDetector = ~(lastElemMask | (lastElemMask - 1));

        int **maskNgs = mallocNeighbourhoodData(arraySize);
        for(int j = 0; j < arraySize; j++) {
            ullong mask = bitMaskArray[i][j];
            if((errorDetector & mask) != 0) {
                printf("...\n");
                exit(-1);
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

            maskNgs[j][0] = maskInd;
            maskNgs[j][1] = maskInd2;
        } 
        result[i] = maskNgs;
        maskNgs = NULL;
    }
    return result;
}

int findThreshold(ullong shapeMask, int m, int k, int*** maskNgsData, ullong** bitMaskArrayData){
    
    int span = -1;

    for(int i = 63; i >= 0; i--) {
        if((shapeMask & (1LL << i)) != 0) {
            span = i + 1;
            break;
        }
    }
   
    if(span == 1) {
        return m - 1;
    }

    //printf("shape span: %d, shape itself: %lld\n", span, shapeMask);

    int** maskNgs = maskNgsData[span-2];
    ullong* bitMaskArray = bitMaskArrayData[span-2];

    int maxInt = 2147483647;

    ullong lastElemMask = 1LL << (span - 1);

    int* binCoef = calculateBinomCoef(k, span);

    int binCoefPrefSum[k+1];

    binCoefPrefSum[0] = binCoef[0];

    for(int i = 1; i <= k; i++) {
        binCoefPrefSum[i] = binCoef[i] + binCoefPrefSum[i-1];
    }

    int arraySize = binCoefPrefSum[k];

    int **currentResult, **nextResult;
    currentResult = mallocIntArrays(binCoefPrefSum, k+1); 
    nextResult = mallocIntArrays(binCoefPrefSum, k+1);
    
    fill(currentResult, binCoefPrefSum, k+1, 0);

    for(int i = span; i <= m; i++) {

        //here we go through all values j and M to compute next iteration    
        for(int j = 0; j <= k; j++) {
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
                               
                ullong fullMask = mask | 1LL;

                ullong xored = fullMask ^ shapeMask;

                int hit = ((xored & ~fullMask) == 0) ? 1 : 0;
                
                int maskInd = maskNgs[l][0];
                int maskInd2 = maskNgs[l][1];

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
                    printf("%d %d\n", maskInd, maskInd2);
                    printf("mask %lld, mask2: %lld, parent: %lld \n", bitMaskArray[maskInd], bitMaskArray[maskInd2], mask);
                    exit(-1);
                } 
            }

        }

        int** tmp = currentResult;
        currentResult = nextResult;
        nextResult = tmp;
    }
   
    int result = currentResult[k][0];

    for(int i = 1; i < arraySize; i++) {
        if(currentResult[k][i] < result) {
            result = currentResult[k][i];
        }
    }
    
    freeIntArrays(currentResult, k+1);
    freeIntArrays(nextResult, k+1);
    free(binCoef);

    return result;
}

struct Pair {
    ullong shape;
    int threshold;
};

boost::lockfree::queue<ullong> workerQueue(0);

boost::lockfree::queue<struct Pair> resultQueue(0);

int m = 50, k = 5;
ullong** bitMaskArrays = precomputeSpanBitMaskArrays(m, k);
int*** maskNgs = precomputeMaskNeighbourhoodMap(m,k,bitMaskArrays);

void worker(void) {
    ullong killCond = 0LL;
    while(true) {
        ullong shape;
        while(!workerQueue.pop(shape));
        if(shape == killCond)
            break;
        int result = findThreshold(shape, m, k, maskNgs, bitMaskArrays);
        struct Pair res;
        res.shape = shape;
        res.threshold = result;
        while(!resultQueue.push(res));
    }

}

int main(){
    
    FILE *f = fopen("result.txt", "w"); 

    boost::thread_group workerThreads; 

    for(int i = 0; i < 4; i++) {
        workerThreads.create_thread(worker);
    }

    unordered_set<ullong> *current = new unordered_set<ullong>(); 
    unordered_set<ullong> *next = new unordered_set<ullong>();
    
    unordered_set<ullong> negatives;

    next->insert(1LL);

    int counter = 0;

    while(!next->empty()){
        printf("next.. size: %d, computed thresholds so far: %d\n", (int) next->size(), counter);
        
        unordered_set<ullong> *tmp = next;
        next = current;
        current = tmp;

        negatives.clear();

        int c = 0;

        for(ullong mask : *current) {
            while(!workerQueue.push(mask));
            c++;
        }
        
        struct Pair res;

        while(c != 0) {
            while(!resultQueue.pop(res));
            c--;
            int ts = res.threshold; 
            counter++;
            ullong mask = res.shape;
            if(ts > 0)
                fprintf(f, "%lld\n", mask);
            for(int i = 0; i < m; i++) {
                ullong tmp = (1LL << i);
                if((tmp & mask) == 0) {
                    ullong nextMask = mask | tmp;
                    // if(nextMask == 7) {
                    //     printf("rootmask %lld, nextMask %lld, rootTS: %d\n", mask, nextMask, ts);
                    // }
                    if(ts == 0) {
                        next->erase(nextMask);
                        negatives.insert(nextMask);
                    } else {
                        if(negatives.find(nextMask) == negatives.end()) {
                            next->insert(nextMask);
                        }
                    }
                }
                        
            }
        } 

        current->clear();

    }

    delete(next);
    delete(current);

    for(int i = 0; i < 4; i++) {
        while(!workerQueue.push(0LL));
    }
    
    workerThreads.join_all();

    fclose(f);
    return 0;
}
