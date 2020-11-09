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

void set_up_packet(uint8_t *data, unsigned int id) {
    unsigned int cpId = id;
    for(int i = 0; i <= 15 ;i++) {
        data[i] = (cpId & 0b1) + '0';
        cpId >>= 1;
    }
}
