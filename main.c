#include <stdio.h>
#include <pthread.h>
#include "args.h"
#include "tpool.h"


int main(int argc, char **argv) {
    struct KerrArgs *args = parse_args(argc, argv);
    if (!args) return 1;
    
    // create thread pool
    struct TPool *mypool = tpool_init(args);


    free_args(args);
}