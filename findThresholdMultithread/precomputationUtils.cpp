#include "precomputationUtils.hpp"
#include "memoryUtils.hpp"
#include "thresholdUtils.hpp"
#include <stdlib.h>
#include <unordered_map>

namespace precomputationUtils {
    using namespace thresholdUtils;
    using namespace std;
    using namespace memoryUtils;

    int** precomputeBinCoefData(int span, int k) {
        
        int** result = (int**) malloc((span-1) * sizeof(int*));

        for(int i = 0; i < span - 1; i++) {
            
            int targetSpan = i + 2;

            int *binCoef = calculateBinomCoef(k, targetSpan);
            
            result[i] = binCoef;
        }
        
        return result;
    }

    int** precomuteBinCoefPrefSumData(int span, int k, int** binCoefData) {
        int** result = (int**) malloc((span-1) * sizeof(int*));

        for(int i = 0; i < span - 1; i++) {
        
            int *prefixSum = (int*) malloc(sizeof(int) * (k+1));

            prefixSum[0] = binCoefData[i][0];

            for(int j = 1; j <= k; j++) {
                prefixSum[j] = binCoefData[i][j] + prefixSum[j-1];
            }

            result[i] = prefixSum;
        }

        return result;
    }

    ullong** precomputeSpanBitMaskArrays(int span, int k, int** binCoefData) {

        ullong** result = (ullong**) malloc((span - 1) * sizeof(ullong*));

        for(int i = 0; i < span - 1; i++) {
            int targetSpan = i + 2;

            int* binCoef = binCoefData[i];

            result[i] = computeBitmaskArray(binCoef, k, targetSpan); 
        }

        return result;
    }

    int*** precomputeMaskNeighbourhoodMap(int span, int k, ullong** bitMaskArray, int** binCoefPrefSum) {
        
        int*** result = (int***) malloc(sizeof(int**) * (span-1));
        
        for(int i = 0; i < span - 1; i++) {
            int targetSpan = i + 2;
            int arraySize = binCoefPrefSum[i][k];
            
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
}
