#include <stdio.h>
#include <pthread.h>
#include "args.h"


int main(int argc, char **argv) {
    KerrArgs *args = parse_args(argc, argv);
    if (!args) return 1;
    
    printf("Num threads: %d          File name: %s\n", args->numThreads, args->fileName);
    free_args(args);
}