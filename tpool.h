#include <stdbool.h>
#include "args.h"


enum Status;
struct Worker;
struct Item;
struct TPool;


struct TPool *tpool_init(struct KerrArgs *args);
static void *worker_driver(void *args);