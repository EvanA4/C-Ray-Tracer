#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "args.h"
#include "tpool.h"
#include "tga.h"


int main(int argc, char **argv) {
    image_writer();
    struct KerrArgs *args = parse_args(argc, argv);
    if (!args) return 1;
    
    // create thread pool
    struct TPool *mypool = tpool_init(args);

    while (1) {
        // read input from user
        char *input = NULL;
        size_t allocd = 0;
        size_t numRead = getline(&input, &allocd, stdin);
        input[numRead - 1] = '\0';

        // send input to worker to print
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