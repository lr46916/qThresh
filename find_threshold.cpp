#include <stdio.h>
#include <stdlib.h>
#include <boost/math/special_functions/binomial.hpp>
#include <unordered_map>
#define min(x,y) x < y ? x : y

using namespace std;

typedef unsigned long long ullong;

ullong* compute_bitmask_array(int* binom_coef_count, int k) {
    
    ullong* result = NULL;
    
    int total = 0;
    for(int i = 0; i < k; i++) {
        total += binom_coef_count[i];
    }

    result = (ullong*) malloc(sizeof(ullong) * total);
    
    ullong bot32_mask = (1LL << 32) - 1;

    int c = 0;
    for(int i = 0; i < k; i++) {
        
        ullong current = 1LL << i;
        current = current | (current - 1);

        for(int j = 0; j < binom_coef_count[i]; j++) {
            result[c++] = (~current) << 1;
            ullong tmp = current | (current - 1); 
            int shift = 0;
            uint input = (uint) current;
            if ((bot32_mask & current) == 0) { 
                input = (uint) (current >> 32);
                shift = 32;
            } 
            shift += __builtin_ctz(input) + 1;
            
            current = (tmp + 1) | (((~tmp & -~tmp) - 1)) >> shift;
        } 
    
    }
    
    return result;
}

int* calculateBinomCoef(int k, int s) {
    
    int* result = (int*) malloc(sizeof(int) * k);

    for(int i = 0; i < k; i++) {
        result[i] = boost::math::binomial_coefficient<double>(s-1,i);
    }
    
    return result;
}

int** malloc_int_arrays(int* row_size, int k){
    
    int** result = (int**) malloc(sizeof(int*) * k);

    for(int i = 0; i < k; i++) {
        result[i] = (int*) malloc(sizeof(int) * row_size[i]);
    }
    
    return result;
}

void free_int_arrays(int**data, int k) {
    for(int i = 0; i < k; i++) {
        free(data[i]);
    }
}

void fill(int** data, int* row_size, int k, int value) {
    for(int i = 0; i < k; i++) {
        for(int j = 0; j < row_size[i]; j++) {
            data[i][j] = value;
        }
    }
}

int find_threshold(int* shape, int size, int m, int k) {
    
    if (m > 64 || size > m || k > size) {
        fprintf(stderr,"Invalid input arguments. shape size: %d, m: %d, k: %d\n", size, m, k);
    }

    //to include k
    k++;

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

    for(int i = 1; i < k; i++) {
        binCoefPrefSum[i] += binCoefPrefSum[i-1];
    }

    int array_size = binCoefPrefSum[k-1];

    ullong* bit_mask_array = compute_bitmask_array(binCoef, k); 
    
    unordered_map<ullong, int> mask_to_index;

    for(int i = 0; i < array_size; i++) {
        mask_to_index[bit_mask_array[i]] = i;
    }

    int **currentResult, **nextResult;
    currentResult = malloc_int_arrays(binCoef, k); 
    nextResult = malloc_int_arrays(binCoef, k);

    for(int i = 0; i < array_size; i++) {
        currentResult[i] = 0;
    }

    ullong lastElemMaskNot = ~lastElemMask;

    for(int i = 0; i < m; i++) {
        
        //not sure about this...
        fill(nextResult, binCoef, k, 0);

        int offset = 0;
        for(int j = 0; j < k; j++) {
            int end = binCoefPrefSum[j];//offset + binCoef[j];

            for(int l = offset; l < end; l++) {
                ullong mask = bit_mask_array[l];
                int target_j = j;
                if ((mask & lastElemMask) != 0) {
                    target_j--;
                }
                ullong target_mask = (mask & lastElemMaskNot) << 1;
                ullong target_mask2 = target_mask | 2LL;
                int mask_ind = mask_to_index[target_mask];
                int mask_ind2 = mask_to_index[target_mask2];
                int hit = (shape_mask & (mask | 1LL)) ? 1 : 0;
                nextResult[j][l] = min(currentResult[target_j][mask_ind2] + hit, currentResult[target_j][mask_ind]);                                     
            }

            offset = end;
        }

        int** tmp = currentResult;
        currentResult = nextResult;
        nextResult = tmp;
    }
   
    int offset = 0;
    if(k > 1) 
        offset = binCoefPrefSum[k-2];

    int result = currentResult[k-1][offset];

    for(int i = offset+1; i < array_size; i++) {
        if(currentResult[k-1][i] < result) {
            result = currentResult[k-1][i];
        }
    }

    free_int_arrays(currentResult, k);
    free_int_arrays(nextResult, k);
    free(bit_mask_array); 
    free(binCoef);

    return result;
}

