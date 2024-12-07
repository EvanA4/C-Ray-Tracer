#include <getopt.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "args.h"


/*
x/t : x pos/dir of camera
y/u : y pos/dir of camera
z/v : z pos/dir of camera
f : camera fov
w : width of picture
h : height of picture
s : task size
m : file name of image
n : number of threads
*/


int free_args(KerrArgs *args) {
    if (!args) return 0;
    if (args->fileName) free(args->fileName);
    free(args);
    return 1;
}


void print_args(KerrArgs *args) {
    printf(
        "Position: {%.2f, %.2f, %.2f}\n"
        "Direction: {%.2f, %.2f, %.2f}\n"
        "FOV: %.2f\n"
        "Image Size: %d x %d\n"
        "Pixels per Task: %d\n"
        "File Name: \"%s\"\n"
        "Number of Threads: %d\n",
        args->pos[0], args->pos[1], args->pos[2],
        args->dir[0], args->dir[1], args->dir[2],
        args->fov,
        args->width, args->height,
        args->taskSize,
        args->fileName,
        args->numThreads
    );
}


KerrArgs *parse_args(int argc, char **argv) {
    KerrArgs *out = malloc(sizeof(KerrArgs));
    *out = (KerrArgs) {
        {0, 10, 0},
        {0, -1, 0},
        60,
        128,
        128,
        256,
        NULL,
        16
    };

    int opt;
    while((opt = getopt(argc, argv, "x:t:y:u:z:v:f:w:h:s:m:n:")) != -1)  
    {  
        switch(opt)  
        {
            case 'x':
                if (sscanf(optarg, "%f", &(out->pos[0])) != 1) {
                    fprintf(stderr, "Error: failed to convert x position to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;
            
            case 't':
                if (sscanf(optarg, "%f", &(out->dir[0])) != 1) {
                    fprintf(stderr, "Error: failed to convert x direction to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;
            
            case 'y':
                if (sscanf(optarg, "%f", &(out->pos[1])) != 1) {
                    fprintf(stderr, "Error: failed to convert y position to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;
            
            case 'u':
                if (sscanf(optarg, "%f", &(out->dir[1])) != 1) {
                    fprintf(stderr, "Error: failed to convert y direction to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'z':
                if (sscanf(optarg, "%f", &(out->pos[2])) != 1) {
                    fprintf(stderr, "Error: failed to convert z position to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'v':
                if (sscanf(optarg, "%f", &(out->dir[2])) != 1) {
                    fprintf(stderr, "Error: failed to convert z direction to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'f':
                if (sscanf(optarg, "%f", &(out->fov)) != 1) {
                    fprintf(stderr, "Error: failed to convert FOV to a float\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'w':
                if (sscanf(optarg, "%d", &(out->width)) != 1) {
                    fprintf(stderr, "Error: failed to convert width to an integer\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'h':
                if (sscanf(optarg, "%d", &(out->height)) != 1) {
                    fprintf(stderr, "Error: failed to convert height to an integer\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 's':
                if (sscanf(optarg, "%d", &(out->taskSize)) != 1) {
                    fprintf(stderr, "Error: failed to convert pixels per task to an integer\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case 'm':
                int argLen = (int) strlen(optarg);
                out->fileName = malloc((argLen + 5) * sizeof(char));
                sprintf(out->fileName, "%s.tga", optarg);
                break;

            case 'n':  
                if (sscanf(optarg, "%d", &(out->numThreads)) != 1) {
                    fprintf(stderr, "Error: failed to convert number of threads to an integer\n");
                    free_args(out);
                    return NULL;
                }
                break;

            case '?':  
                fprintf(
                    stderr,
                    "flags:\n"
                    "\tx/t: %30s\n"
                    "\ty/u: %30s\n"
                    "\tz/v: %30s\n"
                    "\tf: %30s\n"
                    "\tw: %30s\n"
                    "\th: %30s\n"
                    "\ts: %30s\n"
                    "\tm: %30s\n"
                    "\tn: %30s\n",
                    "x pos/dir of camera",
                    "y pos/dir of camera",
                    "z pos/dir of camera",
                    "camera fov",
                    "width of picture",
                    "height of picture",
                    "task size",
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

    print_args(out);
    return out;
}