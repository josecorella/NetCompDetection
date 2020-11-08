#include "mem.h"

char *strarrmem(int len) {
    if (len <= 0) {
        fprintf(stderr, "Invalid memory allocation with size of: %d\n", len);
        exit(EXIT_FAILURE);
    }

    void *mem;
    mem = (char *) malloc(len * sizeof(char *));

    if (mem == NULL) {
        fprintf(stderr, "Memory allocation failed");
        exit(EXIT_FAILURE);
    } else {
        memset(mem, 0, len);
    }
    return mem;
}

int *intarrmem(int len) {
    if (len <= 0) {
        fprintf(stderr, "Invalid memory allocation with size of: %d\n", len);
        exit(EXIT_FAILURE);
    }

    void *mem;
    mem = (int *) malloc(len * sizeof(int *));

    if (mem == NULL) {
        fprintf(stderr, "Memory allocation failed");
        exit(EXIT_FAILURE);
    } else {
        memset(mem, 0, len);
    }
    return mem;
}

uint8_t *unsgnintmem(int len) {
    if (len <= 0) {
        fprintf(stderr, "Invalid memory allocation with size of: %d\n", len);
        exit(EXIT_FAILURE);
    }

    void *mem;
    mem = (uint8_t *) malloc(len * sizeof(uint8_t *));

    if (mem == NULL) {
        fprintf(stderr, "Memory allocation failed");
        exit(EXIT_FAILURE);
    } else {
        memset(mem, 0, len);
    }
    return mem;
}

