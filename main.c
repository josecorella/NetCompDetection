#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h> 
#include <string.h>
#include <arpa/inet.h>

#include "parse_json.h"
#include "mem.h"
#include "construct_packet.h"

#define SA struct sockaddr

void set_up_socket(struct json config, int sockeType, int *socketFd, struct sockaddr_in *client) {
    *socketFd = socket(AF_INET, sockeType, 0);

    if (*socketFd == -1) {
        fprintf(stderr, "Failed to set up socket connection");
        exit(EXIT_FAILURE);
    }

    /* 0 out client struct */
    memset(client, 0, sizeof(*client));

    client->sin_family = AF_INET;
    client->sin_addr.s_addr = inet_addr(config.serverIp); 
    client->sin_port = htons(atoi(config.tcpPort));

}

int main (int argc, char **argv) {
    struct json config;
    int socketFd, connFd, val, clientLen; 
    struct sockaddr_in client, cli;
    uint8_t *preProbe, *probe;

    /* Pre Probing phase */
    if (argc != 2) {
        fprintf(stderr, "Usage: ./compdetectclient <config file>\n");
        return EXIT_FAILURE;
    }

    read_json(&config, argv[1]);

    preProbe = unsgnintmem(config.payloadSize);

    /* Set up socket connection */
    //set_up_socket(config, SOCK_STREAM, &socketFd, &client);

    /* Load Entropy */
    entropy(&preProbe[16], config.payloadSize - 16);

    printf("Entropy: %s\n", preProbe);

    

}
