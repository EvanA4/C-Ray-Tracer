#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
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
    char *tempStr;
    Item *next;
};


struct Queue {
    Item *head;
    pthread_mutex_t mutex;
    pthread_cond_t notEmpty;
    pthread_cond_t notFull;
};


struct TPool {
    Worker *workers;
    Queue *q;
    int numWorkers;
};


struct WorkerArgs {
    TPool *tpool;
    int tidx;
};


static bool q_isempty(Queue *q) {
    return q->head->next == NULL;
}


static void q_push(Queue *q, Item *toAdd) {
    if (!q || !toAdd) return;

    Item *backup = q->head;
    while (q->head->next) {
        q->head = q->head->next;
    }
    q->head->next = toAdd;
    q->head = backup;
}


static Item *q_pop(Queue *q) {
    if (!q || q_isempty(q)) return NULL;

    Item *out = q->head->next;
    if (out->next) q->head->next = out->next;
    else q->head->next = NULL;
    return out;
}


void *worker_driver(void *args) {
    WorkerArgs *wargs = (WorkerArgs *) args;
    Worker *me = &(wargs->tpool->workers[wargs->tidx]);
    Queue *q = wargs->tpool->q;

    while (me->stat != DYING) {
        // have worker wait for new task
        while (q_isempty(q) && me->stat != DYING) {
            pthread_mutex_lock(&(q->mutex));
            pthread_cond_wait(&(q->notEmpty), &(q->mutex));
            pthread_mutex_unlock(&(q->mutex));
        }
        if (me->stat == DYING) break;

        // actually do the work
        me->stat = BUSY;
        Item *cur = q_pop(q);
        printf("[%d] \"%s\"\n", wargs->tidx, cur->tempStr);
        free(cur->tempStr);
        free(cur);
        me->stat = IDLE;
    }

    free(wargs);
    return NULL;
}


TPool *tpool_init(KerrArgs *args) {
    TPool *out = malloc(sizeof(TPool));
    out->workers = malloc(args->numThreads * sizeof(Worker));

    // set workers to idle
    for (int i = 0; i < args->numThreads; ++i) {
        out->workers[i].stat = IDLE;
    }

    out->numWorkers = args->numThreads;
    out->q = malloc(sizeof(Queue));
    out->q->head = malloc(sizeof(Item));
    out->q->head->next = NULL;

    // initialize mutex and conditional variables
    pthread_mutex_init(&(out->q->mutex), NULL);
    pthread_cond_init(&(out->q->notFull), NULL);
    pthread_cond_init(&(out->q->notEmpty), NULL);

    // create actual threads and give them access to TPool
    for (int i = 0; i < args->numThreads; ++i) {
        WorkerArgs *wargs = malloc(sizeof(WorkerArgs));
        wargs->tpool = out;
        wargs->tidx = i;
        pthread_create(&(out->workers[i].tid), NULL, worker_driver, (void *) wargs);
    }

    return out;
}


void tpool_push(TPool *tpool, char *toPrint) {
    Item *toAdd = malloc(sizeof(Item));
    toAdd->tempStr = toPrint;
    toAdd->next = NULL;

    pthread_mutex_lock(&(tpool->q->mutex));
    q_push(tpool->q, toAdd);
    pthread_cond_signal(&(tpool->q->notEmpty));
    pthread_mutex_unlock(&(tpool->q->mutex));
}


void tpool_close(TPool *tpool) {
    if (!tpool) return;

    // tell all workers to die and signal them
    for (int i = 0; i < tpool->numWorkers; ++i) {
        tpool->workers[i].stat = DYING;
    }
    pthread_mutex_lock(&(tpool->q->mutex));
    pthread_cond_broadcast(&(tpool->q->notEmpty));
    pthread_mutex_unlock(&(tpool->q->mutex));

    // wait for workers to die
    for (int i = 0; i < tpool->numWorkers; ++i) {
        pthread_join(tpool->workers[i].tid, NULL);
    }

    free(tpool->workers);
    free(tpool->q->head);
    pthread_mutex_destroy(&(tpool->q->mutex));
    pthread_cond_destroy(&(tpool->q->notEmpty));
    pthread_cond_destroy(&(tpool->q->notFull));
    free(tpool->q);
    free(tpool);
}