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
#define MAXDATA 1024

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
    char data[MAXDATA] = {0};

    /* Pre Probing phase */
    
    if (argc != 2) {
        fprintf(stderr, "Usage: ./compdetectclient <config file>\n");
        return EXIT_FAILURE;
    }

    read_json(&config, argv[1], data);


    /* Set up socket connection */
    set_up_socket(config, SOCK_STREAM, &socketFd, &client);


    /* Try Connecting to server */
    if (connect(socketFd, (SA*) &client, sizeof(client)) != 0) {
        fprintf(stderr, "Failure to Connect to Server");
        return EXIT_FAILURE;
    } else {
        LOGP("Connection Successful!! :)\n");
    }

    /* Attempt to send data to server */
    if (send(socketFd, data, MAXDATA - 1, 0) == -1) {
        fprintf(stderr, "Unable to send data! Exiting...\n");
        return EXIT_FAILURE;
    }

    /* Check if we can receive data from server after sending */
    char success[8];
    memset(success, 0, 8);
    if (recv(socketFd, success, 8, 0) == -1) {
        fprintf(stderr, "Error reciving confirmation from server. Exiting...\n");
        return EXIT_FAILURE;
    }

    if (strncmp(success, "success", 7) != 0) {
        fprintf(stderr, "PreProbing Failed!\n");
        return EXIT_FAILURE;
    }

    /* If we are up to this point pre-probing phase is complete; close the TCP connection */
    close(socketFd);



    /* Probing Phase - Send UDP Train */


    preProbe = unsgnintmem(config.payloadSize);

    
    /* Load Entropy */
    entropy(&preProbe[16], config.payloadSize - 16);


    

}
