#ifndef TPOOL_H
#define TPOOL_H


#include <stdbool.h>
#include "args.h"


typedef enum Status Status;
typedef struct Worker Worker;
typedef struct Item Item;
typedef struct TPool TPool;


TPool *tpool_init(KerrArgs *args);
void *worker_driver(void *args);


#endif