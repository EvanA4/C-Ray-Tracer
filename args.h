struct KerrArgs {
    int numThreads;
    char *fileName;
};

int free_args(struct KerrArgs *args);
struct KerrArgs *parse_args(int argc, char **argv);