#include <getopt.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "args.h"


/*
width of picture
height of picture
batch size
task size
*/


int free_args(KerrArgs *args) {
    if (!args) return 0;
    if (args->fileName) free(args->fileName);
    free(args);
    return 1;
}


KerrArgs *parse_args(int argc, char **argv) {
    KerrArgs *out = malloc(sizeof(KerrArgs));
    *out = (KerrArgs) {16, NULL};

    int opt;
    while((opt = getopt(argc, argv, "t:f:")) != -1)  
    {  
        switch(opt)  
        {
            case 't':  
                if (sscanf(optarg, "%d", &(out->numThreads)) != 1) {
                    fprintf(stderr, "Error: failed to convert number of threads to an integer\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'f':
                int argLen = (int) strlen(optarg);
                out->fileName = malloc((argLen + 5) * sizeof(char));
                sprintf(out->fileName, "%s.tga", optarg);
                break;

            case '?':  
                fprintf(
                    stderr,
                    "flags:\n"
                    "\tf: %30s\n"
                    "\tt: %30s\n",
                    "file name of image",
                    "number of threads"
                ); 
                free_args(out);
                return NULL;
        }  
    }

    if (!out->fileName) {
        out->fileName = malloc(64 * sizeof(char));
        time_t seconds = time(NULL);
        strftime(out->fileName, 64, "%m-%d-%Y.tga", localtime(&seconds));
    }

    return out;
}