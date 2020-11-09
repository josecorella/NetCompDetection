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
#include "logger.h"

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
    int socketFd, val, clientLen; 
    struct sockaddr_in client, cli;
    uint8_t *preProbe, *probe;
    char data[1024] = {0};

    /* Pre Probing phase */
    if (argc != 2) {
        fprintf(stderr, "Usage: ./compdetectclient <config file>\n");
        return EXIT_FAILURE;
    }

    read_json(&config, argv[1], data);

    preProbe = unsgnintmem(config.payloadSize);

    /* Set up socket connection */
    //set_up_socket(config, SOCK_STREAM, &socketFd, &client);

    /* Load Entropy */
    entropy(&preProbe[16], config.payloadSize - 16);

    /* Try Connecting to server */
    if (connect(socketFd, (SA*) &client, sizeof(client)) != 0) {
        fprintf(stderr, "Failure to Connect to Server");
        return EXIT_FAILURE;
    } else {
        LOGP("Connection Successful!! :)\n");
    }

    /* Attempt to send data to server */
    


    

}
