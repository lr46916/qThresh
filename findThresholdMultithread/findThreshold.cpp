#include <stdio.h>
#include <stdlib.h>
#include <climits>
#include "findThreshold.hpp"

typedef unsigned long long ullong;

//fill all elements of 2D array with given value 
//@param data 2D array to fill
void fill(int** data, int* rowSize, int k, int value) {
    for(int i = 0; i < k; i++) {
        for(int j = 0; j < rowSize[i]; j++) {
            data[i][j] = value;
        }
    }
}

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
int findThreshold(ullong shapeMask, int span, int m, int k, int** maskNgs, unsigned long long *bitMaskArray, int* binCoefPrefSum, int** currentResult, int** nextResult) {

    if(span == 1) {
        return m - 1;
    }

    ullong lastElemMask = 1LL << (span - 1);

    int arraySize = binCoefPrefSum[k];

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

                int nextVal = INT_MAX;
                
                //if target mask has more then targetJ missmatches it is invalid
                if(maskInd != -1 && maskInd < binCoefPrefSum[targetJ]){
                    nextVal = currentResult[targetJ][maskInd];
                }

                if(maskInd2 != -1 && maskInd2 < binCoefPrefSum[targetJ]) {
                    int val = currentResult[targetJ][maskInd2] + hit;
                    if(val < nextVal)
                        nextVal = val;
                }

                if(nextVal != INT_MAX){
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
    
    return result;
}
