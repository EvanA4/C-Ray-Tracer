#include <pthread.h>
#include <stdlib.h>
#include "tpool.h"
#include "args.h"


enum Status {
    BUSY,
    IDLE,
    DYING
};


struct Worker {
    pthread_t tid;
    Status stat;
};


struct Item {
    bool isTaken;
    int startPx;
    int numPx;
    Item *next;
};


struct TPool {
    Worker *workers;
    Item *queue;
};


void *worker_driver(void *args) {
    return NULL;
}


TPool *tpool_init(KerrArgs *args) {
    TPool *out = malloc(sizeof(TPool));
    out->workers = malloc(args->numThreads * sizeof(Worker));

    // create actual threads and give them access to TPool
    for (int i = 0; i < args->numThreads; ++i) {
        pthread_create(&(out->workers[i].tid), NULL, worker_driver, (void *) out);
    }

    return out;
}