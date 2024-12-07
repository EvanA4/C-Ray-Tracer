#ifndef ARGS_H
#define ARGS_H


typedef struct KerrArgs {
    int numThreads;
    char *fileName;
} KerrArgs;


int free_args(KerrArgs *args);
KerrArgs *parse_args(int argc, char **argv);


#endif