#include <stdio.h>
#include <stdlib.h>
#include <unordered_set>
#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include "findThreshold.hpp" 
#include "precomputationUtils.hpp"
#include "memoryUtils.hpp"

//number of worker threads that will be used in threshold computation.
#define NTHREADS 3

typedef unsigned long long ullong;

struct Pair {
    ullong shape;
    int threshold;
};

boost::lockfree::queue<ullong> workerQueue(1000);

boost::lockfree::queue<struct Pair> resultQueue(1000);

int m = -1, k = -1;

//precomputed data is held globally so all threads can easely acces it.
int** binCoefPrefSumData;
ullong** bitMaskArrays; 
int*** maskNgs;
int*** preallocatedDataStorage;

//Worker method. Each worker thread runs this method as long as killCond isn't received.
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

int main(int argc, char *argv[]){
 
    m = 50;
    k = 5;
    int maxSpan = 45;

    //precomputig all data needed for DP recurrence.
    int** binCoefData = precomputationUtils::precomputeBinCoefData(maxSpan,k);
    binCoefPrefSumData = precomputationUtils::precomuteBinCoefPrefSumData(maxSpan,k,binCoefData);
    bitMaskArrays = precomputationUtils::precomputeSpanBitMaskArrays(maxSpan,k,binCoefData);
    maskNgs = precomputationUtils::precomputeMaskNeighbourhoodMap(maxSpan,k,bitMaskArrays, binCoefPrefSumData);
    memoryUtils::free2DArray(binCoefData, maxSpan-1);

    //file in which computed thresholds are stored.
    FILE *f = fopen("result.txt", "w");

    //each thread needs its own memory to store data while computing threshold.
    preallocatedDataStorage = memoryUtils::preallocateDPArrays(k,binCoefPrefSumData[maxSpan-2], NTHREADS);

    boost::thread_group workerThreads;
    
    //creating workers...
    for(int i = 0; i < NTHREADS; i++) {
        workerThreads.create_thread(boost::bind(worker,i));
    }
    
    //set containing all shape masks that are current targets (possible positive thresholds)
    std::unordered_set<ullong> *current = new std::unordered_set<ullong>();

    //set containing all shape masks that will be checked in next iteration computed from current shapes.
    std::unordered_set<ullong> *next = new std::unordered_set<ullong>();

    if(argc == 2) {
        FILE *resumeFile = fopen(argv[1], "r");
        
        ullong tmp = 0;

        while(fscanf(resumeFile, "%lld ", &tmp) == 1) {
            next->insert(tmp);
        }
    } else {
        //start with 1...
        next->insert(1LL);
    }


    //once we find a shape with non positive threshold we want to make sure all
    //shapes that are his supersets are ignored.
    std::unordered_set<ullong> *negatives = new std::unordered_set<ullong>();

    

    int counter = 0;
    
    //this loop go through all shapes (with fixed values of m and k) starting with shape that has
    //q = 1. Then in each iteration examine all shapes with q=2, then q=3...
    //If in current iteration (say q=q') we find a shape with threshold=0 we ignore all shapes with
    //q = q'+1 that are super set of current shape. And with that reducing our search space.
    while(!next->empty()){
        printf("next.. size: %d, computed thresholds so far: %d\n", (int) next->size(), counter);

        FILE *resumeFile = fopen("resumeFile.txt", "w");
        for(ullong mask : *next) {
            fprintf(resumeFile, "%lld ", mask);
        }
        fclose(resumeFile);

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
            for(int i = 0; i < maxSpan; i++) {
                ullong tmp = (1LL << i);
                if((tmp & mask) == 0) {
                    ullong nextMask = mask | tmp;
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
    
    fflush(f);
    fclose(f);
    
    //free everything...
    delete(next);
    delete(current);
    delete(negatives);
    for(int i = 0; i < maxSpan - 1; i++) {
        memoryUtils::free2DArray(maskNgs[i], binCoefPrefSumData[i][k]);
    }
    free(maskNgs);
    memoryUtils::free2DArray(bitMaskArrays, maxSpan-1);
    memoryUtils::free2DArray(binCoefPrefSumData, maxSpan-1);

    return 0;
}
