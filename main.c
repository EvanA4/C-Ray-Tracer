#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "args.h"
#include "tpool.h"


int main(int argc, char **argv) {
    struct KerrArgs *args = parse_args(argc, argv);
    if (!args) return 1;
    
    // create thread pool
    struct TPool *mypool = tpool_init(args);

    // read input from user
    while (1) {
        char *input = NULL;
        size_t allocd = 0;
        size_t numRead = getline(&input, &allocd, stdin);
        input[numRead - 1] = '\0';

        if (!strcmp(input, "exit")) {
            free(input);
            tpool_close(mypool);
            break;
        } else {
            tpool_push(mypool, input);
        }
    }


    free_args(args);
}