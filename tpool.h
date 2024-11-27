#ifndef TPOOL_H
#define TPOOL_H


#include <stdbool.h>
#include "args.h"


typedef enum Status Status;
typedef struct Worker Worker;
typedef struct Item Item;
typedef struct Queue Queue;
typedef struct TPool TPool;
typedef struct WorkerArgs WorkerArgs;


TPool *tpool_init(KerrArgs *args);
void *worker_driver(void *args);
void tpool_push(TPool *tpool, char *toPrint);
void tpool_close(TPool *tpool);


#endif