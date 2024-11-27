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
    enum Status stat;
};


struct Item {
    bool isTaken;
    int startPx;
    int numPx;
    struct Item *next;
};


struct TPool {
    struct Worker *workers;
    struct Item *queue;
};


static void *worker_driver(void *args) {
    return NULL;
}


struct TPool *tpool_init(struct KerrArgs *args) {
    struct TPool *out = malloc(sizeof(struct TPool));
    out->workers = malloc(args->numThreads * sizeof(struct Worker));

    // create actual threads and give them access to TPool
    for (int i = 0; i < args->numThreads; ++i) {
        pthread_create(&(out->workers[i].tid), NULL, worker_driver, (void *) out);
    }

    return out;
}