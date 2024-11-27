typedef struct KerrArgs {
    int numThreads;
    char *fileName;
} KerrArgs;

int free_args(struct KerrArgs *args);
KerrArgs *parse_args(int argc, char **argv);