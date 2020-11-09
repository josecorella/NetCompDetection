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

#define MAXDATA 1024
#define IP_MTU_DISCOVER 10
#define IP_PMTUDISC_DO 2


int main (int argc, char **argv) {
    struct json config;
    int socketFd, val, addressLen, udpFd, df;
    unsigned int packetId = 0;
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
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        fprintf(stderr, "Failed to set up socket connection");
        exit(EXIT_FAILURE);
    }

    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr(config.serverIp); 
    client.sin_port = htons(atoi(config.tcpPort));

    /* Try Connecting to server */
    if (connect(socketFd, (struct sockaddr *) &client, sizeof(client)) != 0) {
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
    randomData = unsgnintmem(config.payloadSize);
    randomData2 = unsgnintmem(config.payloadSize);
    entropy(&randomData2[16], config.payloadSize - 16);

    addressLen = sizeof(address);
    sendto(udpFd, randomData, config.payloadSize, 0, (struct sockaddr *) &address, addressLen);
    
    /*Send Low Entropy Data*/
    for (int i = 0; i < config.numPackets; i++) {
        set_up_packet(randomData, packetId++);
        if (sendto(udpFd, randomData, config.payloadSize, 0, (struct sockaddr *) &address, addressLen) <= 0) {
            continue;
        }
    }

    sleep(config.msrTime);

    /* Send High Entropy Data */
    for (int i = 0; i < config.numPackets; i++) {
        set_up_packet(randomData2, packetId++);
        if (sendto(udpFd, randomData2, config.payloadSize, 0, (struct sockaddr *) &address, addressLen) <= 0) {
            continue;
        }
    }

    printf("Packets Sent, closing connection in 3, 2, 1 ...\n");
    close(udpFd);

    sleep(10);

    /* IF WE HAVE MADE IT THIS FAR WE ARE IN PHASE 3 */
    
    struct sockaddr_in serverAddress;
    
    //create another socket to report the findings of the server
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        fprintf(stderr, "Could not set up socket :(\n");
        return EXIT_FAILURE;
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(config.serverIp);
    serverAddress.sin_port = htons(8082);

    //time to connect client socket to server socket to see if there is any compression
    if (connect(socketFd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        fprintf(stderr, "Could not connect to Server :(\n");
        return EXIT_FAILURE;
    }

    memset(&data, 0, MAXDATA);
    send(socketFd, data, MAXDATA, 0);
    memset(&data, 0, MAXDATA);
    recv(socketFd, data, MAXDATA, 0);
    close(socketFd);

    //Did we have compression???
    printf("%s\n", data);
    free(randomData);
    free(randomData2);

    return 0;
}

