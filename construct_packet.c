#include "construct_packet.h"

void entropy(uint8_t *entropy, int len) {
    FILE* fptr = NULL;

    fptr = fopen("/dev/random", "r");

    if (fptr == NULL) {
        fprintf(stderr, "Unable to read /dev/random\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < len - 1; i++) {
        entropy[i] = getc(fptr);
    }

    fclose(fptr);

}

