#include <stdio.h>
#include <stdlib.h>
#include <unordered_set>
#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include "findThreshold.hpp" 
#include "precomputationUtils.hpp"
#include "memoryUtils.hpp"

#define NTHREADS 3

using namespace memoryUtils;

typedef unsigned long long ullong;

struct Pair {
    ullong shape;
    int threshold;
};

boost::lockfree::queue<ullong> workerQueue(1000);

boost::lockfree::queue<struct Pair> resultQueue(1000);

int m = -1, k = -1;

int** binCoefPrefSumData;
ullong** bitMaskArrays; //= precomputeSpanBitMaskArrays(m, k);
int*** maskNgs; //precomputeMaskNeighbourhoodMap(m,k,bitMaskArrays);
int*** preallocatedDataStorage;

void worker(int id) {
    ullong killCond = 0LL;
    while(true) {
        ullong shape;
        while(!workerQueue.pop(shape));
        if(shape == killCond)
            break;
        int span = 64 - __builtin_clzll(shape);
        int result = findThreshold(shape, span, m, k, maskNgs[span-2], bitMaskArrays[span-2], binCoefPrefSumData[span-2], preallocatedDataStorage[2*id], preallocatedDataStorage[2*id+1]);
        struct Pair res;
        res.shape = shape;
        res.threshold = result;
        while(!resultQueue.push(res));
    }

}

int main(){
 
    m = 50;
    k = 5;
    
    int** binCoefData = precomputationUtils::precomputeBinCoefData(m,k);
    binCoefPrefSumData = precomputationUtils::precomuteBinCoefPrefSumData(m,k,binCoefData);
    bitMaskArrays = precomputationUtils::precomputeSpanBitMaskArrays(m,k,binCoefData);
    maskNgs = precomputationUtils::precomputeMaskNeighbourhoodMap(m,k,bitMaskArrays, binCoefPrefSumData);
    memoryUtils::free2DArray(binCoefData, m-1);

    FILE *f = fopen("result.txt", "w"); 

    preallocatedDataStorage = memoryUtils::preallocateDataArraysForDP(k,binCoefPrefSumData[m-2], NTHREADS);

    boost::thread_group workerThreads; 

    for(int i = 0; i < NTHREADS; i++) {
        workerThreads.create_thread(boost::bind(worker,i));
    }

    std::unordered_set<ullong> *current = new std::unordered_set<ullong>(); 
    std::unordered_set<ullong> *next = new std::unordered_set<ullong>();
    
    std::unordered_set<ullong> *negatives = new std::unordered_set<ullong>();

    next->insert(1LL);

    int counter = 0;
    int tmpC = 0;

    while(!next->empty()){
        if(tmpC++ == 4)
            break;
        printf("next.. size: %d, computed thresholds so far: %d\n", (int) next->size(), counter);

        std::unordered_set<ullong> *tmp = next;
        next = current;
        current = tmp;

        negatives->clear();

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
                fprintf(f, "%lld %d\n", mask, ts);
            for(int i = 0; i < m; i++) {
                ullong tmp = (1LL << i);
                if((tmp & mask) == 0) {
                    ullong nextMask = mask | tmp;
                    // if(nextMask == 7) {
                    //     printf("rootmask %lld, nextMask %lld, rootTS: %d\n", mask, nextMask, ts);
                    // }
                    if(ts == 0) {
                        next->erase(nextMask);
                        negatives->insert(nextMask);
                    } else {
                        if(negatives->find(nextMask) == negatives->end()) {
                            next->insert(nextMask);
                        }
                    }
                }
                        
            }
        } 

        current->clear();

    }

    for(int i = 0; i < NTHREADS; i++) {
        while(!workerQueue.push(0LL));
    }
    
    workerThreads.join_all();

    fclose(f);
    
    //free everything...
    delete(next);
    delete(current);
    delete(negatives);
    for(int i = 0; i < m - 1; i++) {
        memoryUtils::free2DArray(maskNgs[i], binCoefPrefSumData[i][k]);
    }
    free(maskNgs);
    memoryUtils::free2DArray(bitMaskArrays, m-1);
    memoryUtils::free2DArray(binCoefPrefSumData, m-1);

    return 0;
}
