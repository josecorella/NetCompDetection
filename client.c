#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h> 
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "parse_json.h"
#include "mem.h"
#include "construct_packet.h"
#include "logger.h"

#define SA struct sockaddr
#define MAXDATA 1024
#define IP_MTU_DISCOVER 10
#define IP_PMTUDISC_DO 2

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
    int socketFd, val, clientLen, udpFd, df; 
    struct sockaddr_in client, cli, address, clientSrc;
    uint8_t *randomData, *randomData2;
    char data[MAXDATA] = {0};

    /* Pre Probing phase */
    
    if (argc != 2) {
        fprintf(stderr, "Usage: ./compdetectclient <config file>\n");
        return EXIT_FAILURE;
    }

    /* Read Config */
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
    
    randomData = unsgnintmem(config.payloadSize);
    
    /* Load Entropy */
    entropy(&randomData[16], config.payloadSize - 16); //Why at 16? see the UDP packet structure in spec
    
    udpFd = socket(AF_INET, SOCK_DGRAM, 0);

    if (udpFd < 0) {
        fprintf(stderr, "Failure setting up UDP Socket :(\n");
        return EXIT_FAILURE;
    }

    /* Set up struct for sending packets to server */
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(config.serverIp);
    address.sin_port = htons(atoi(config.dstPortUdp));

    /* Set up struct for recieving packets from any source once we bind to the socket */
    memset(&clientSrc, 0, sizeof(clientSrc));
    clientSrc.sin_family = AF_INET;
    clientSrc.sin_addr.s_addr = htonl(INADDR_ANY); //why INADDR_ANY?  in the bind call, the socket will be bound to all local interfaces
    clientSrc.sin_port = htons(atoi(config.srcPortUdp));
    df = IP_PMTUDISC_DO;

    /* Set up Don't Fragment Bit see https://www.programmersought.com/article/48555483026/ for approach */
    if (setsockopt(udpFd, IPPROTO_IP, IP_MTU_DISCOVER, &df, sizeof(df)) == -1) {
        fprintf(stderr, "Don't Fragment bit not set :(\n");
        return EXIT_FAILURE;
    }

    /* Bind Socket */
    if (bind(udpFd, (struct sockaddr *) &clientSrc, sizeof(clientSrc)) == -1) {
        fprintf(stderr, "Failed to Bind to socket :(\n");
        return EXIT_FAILURE;
    }

    /* Connect Socket */
    if (connect(udpFd, (struct sockaddr *) &address, sizeof(address)) == -1) {
        fprintf(stderr, "Failed to connect to socket :(\n");
        return EXIT_FAILURE;
    }

    /* IF WE HAVE MADE IT THIS FAR WE CAN START SENDING PACKETS OVER!!! WOOT WOOT */

    /* Send low entropy packets */
    int addressLen = sizeof(address);







    


    

}
