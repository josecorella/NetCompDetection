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

/* Prototypes */

int set_up_server(int *socketFd, int sockType, struct sockaddr_in *server, struct sockaddr_in *client);
void send_response(int connection, char *data, int dataLen, struct json *tcpInfo);
int load_json(char *data, struct json *tcpInfo);

int set_up_server(int *socketFd, int sockType, struct sockaddr_in *server, struct sockaddr_in *client) {
    int connectionFd;
    unsigned int clientSize;

    // open a socket
    *socketFd = socket(AF_INET, sockType, 0);
    if (*socketFd == -1) {
        fprintf(stderr, "Failed creating socket :(\n");
        exit(EXIT_FAILURE);
    }
    // set up server socket struct
    bzero(server, sizeof(*server)); // memset make this go break, no memset, bad memset
    server->sin_family = AF_INET;
    server->sin_addr.s_addr = htonl(INADDR_ANY); //in the bind call, the socket will be bound to all local interfaces
    server->sin_port = htons(8081);

    // bind the socket
    if (bind(*socketFd, (struct sockaddr *) server, sizeof(*server)) == -1) {
        fprintf(stderr, "Failed to Bind\n");
        exit(EXIT_FAILURE);
    }
    LOGP("Successfully Binded\n");

    // time to listen to connections
    if (listen(*socketFd, 5) == -1) {
        fprintf(stderr, "Failed to listen on socket: %d\n", *socketFd);
        exit(EXIT_FAILURE);
    }

    // Recieve data from client
    clientSize = sizeof(*client);
    connectionFd = accept(*socketFd, (struct sockaddr *) client, &clientSize);
    if (connectionFd == -1) {
        fprintf(stderr, "Failed to receive data on socket: %d\n", *socketFd);
        exit(EXIT_FAILURE);
    }

    return connectionFd;
}

void send_response(int connection, char *data, int dataLen, struct json *tcpInfo) {
    //THIS RESPONSE IS TO MAKE SURE WE GOT THE INFO FOR TCP AND UDP CONNECTION
    char failure[8] = "Failed!";
    char success[8] = "Success";

    memset(data, 0, MAXDATA);

    recv(connection, data, MAXDATA, 0); //recieve data from socket and store it in data
    LOG("Data Received: %s\n", data); // since this is the first connection we assume it is the tcp udp info
    if (load_json(data, tcpInfo) != 0) {
        // failed to recieve right info
        send(connection, failure, 8, 0);

        //close socket
        close(connection);
        exit(EXIT_FAILURE);
    }

    send(connection, success, 8, 0);
    
}

int load_json(char *data, struct json *tcpInfo) {
    // go through data and check if it matches what we are looking for if not return -1
    
    // check for white space at the beginning
    while (*data == ' ') {
        data += 1;
    }

    // fortunately with this approach I know what to expect so I can have a somewhat expected value
    // do a bunch of strsep to get the info
    tcpInfo->serverIp = strsep(&data, " ");
    tcpInfo->srcPortUdp = strsep(&data, " ");
    tcpInfo->dstPortUdp = strsep(&data, " ");
    tcpInfo->dstPortTcpHead = strsep(&data, " ");
    tcpInfo->dstPortTcpTail = strsep(&data, " ");
    tcpInfo->tcpPort = strsep(&data, " ");
    tcpInfo->payloadSize = atoi(strsep(&data, " "));
    tcpInfo->msrTime = atoi(strsep(&data, " "));
    tcpInfo->numPackets = atoi(strsep(&data, " "));
    tcpInfo->ttl = atoi(strsep(&data, " "));

    // Hopefully we have the right data, cross fingers! Should check if any are null here. 
    return 0;
}

int main (int argc, char **argv) {
    int socketFd, connectionFd, udpConn;
    unsigned int clientLen;
    int numBytes;
    clock_t start, end;
    double deltaTime, lowEntropy, highEntropy;
    struct sockaddr_in serverAddress, client; // TCP Connection
    struct sockaddr_in clientAddress;   //UDP Connection
    struct sockaddr_in serverAddressResult, clientAddressResult;    // TCP Connection Results
    struct json tcpInfo;
    char data[MAXDATA] = {0};
    char result[26] = {0};

    /*Set up connection so that we get can listen */
    connectionFd = set_up_server(&socketFd, SOCK_STREAM, &serverAddress, &client);
    send_response(connectionFd, data, MAXDATA, &tcpInfo);
    print_json_s(&tcpInfo);
    close(socketFd);
    //got tcp info time to close socket and move on

    sleep(10); //give server some rest

    char *udpData = calloc(tcpInfo.payloadSize, sizeof(char)); //get some zeroed out memory for our packets that we'll receive and send back
    udpConn = socket(AF_INET, SOCK_DGRAM, 0);

    if (udpConn == -1) {
        fprintf(stderr, "Socket creation failed!!\n");
        return EXIT_FAILURE;
    }
    
    // zero out our struct
    bzero(&clientAddress, sizeof(clientAddress));

    // set struct values so that we can receive appropriate info
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddress.sin_port = htons(atoi(tcpInfo.dstPortUdp));

    // time to  bind
    if (bind(udpConn, (struct sockaddr *) &clientAddress, sizeof(clientAddress)) == -1) {
        fprintf(stderr, "Failed to bind :(\n");
        return EXIT_FAILURE;
    }

    // now that we have binded the socket we should listen in and see if we get the UDP packets we were promised
    clientLen = sizeof(clientAddress);
    recvfrom(udpConn, (char *) udpData, tcpInfo.payloadSize, MSG_WAITALL, (struct sockaddr *) &clientAddress, &clientLen);
    
    // should probs check if recvfrom is -1

    //start the clock (low entropy packets)!
    start = clock();
    for (int i = 0; i < tcpInfo.numPackets; i++) {
       recvfrom(udpConn, (char *) udpData, tcpInfo.payloadSize, MSG_WAITALL, (struct sockaddr *) &clientAddress, &clientLen);
    }
    end = clock();
    deltaTime = (((double) end) - ((double) start)) / ((double) CLOCKS_PER_SEC);
    lowEntropy = deltaTime * 1000; // this is how long it took to send x packets of low entropy
    LOG("Time for Low Entropy: %f.", lowEntropy);
    
    sleep(tcpInfo.msrTime); // wait the interval

    //start the clock (high entropy packets)!
    start = clock();
    for (int i = 0; i < tcpInfo.numPackets; i++) {
       recvfrom(udpConn, (char *) udpData, tcpInfo.payloadSize, MSG_WAITALL, (struct sockaddr *) &clientAddress, &clientLen);
    }
    end = clock();
    deltaTime = (((double) end) - ((double) start)) / ((double) CLOCKS_PER_SEC);
    highEntropy = deltaTime * 1000; // this is how long it took to send x packets of high entropy
    LOG("Time for High Entropy: %f.", highEntropy);

    // finished getting packets time to close and number crunch
    close(udpConn);
    sleep(15); // take a breather, you worked hard

    // Tell the client if we found compressio or not :)
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        fprintf(stderr, "Failed to create Socket!\n");
        return EXIT_FAILURE;
    }
    
    // zero out a new struct
    bzero(&serverAddressResult, sizeof(serverAddressResult));
    
    serverAddressResult.sin_family = AF_INET;
    serverAddressResult.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddressResult.sin_port = htons(8082);

    // time to  bind
    if (bind(socketFd, (struct sockaddr *) &serverAddressResult, sizeof(serverAddressResult)) == -1) {
        fprintf(stderr, "Failed to bind :(\n");
        return EXIT_FAILURE;
    }

    // time to listen in
    if (listen(socketFd, 5) == -1) {
        fprintf(stderr, "Server unable to listen\n");
        return EXIT_FAILURE;
    }
    
    //accept packets from client
    clientLen = sizeof(clientAddressResult);
    connectionFd = accept(socketFd, (struct sockaddr *) &clientAddressResult, &clientLen);
    
    // if we were not able to receive we cry
    if (connectionFd == -1) {
        fprintf(stderr, "Unable to receive data\n");
        return EXIT_FAILURE;
    }

    // now it is time for the wonderful formula from the spec!! YESSS WE MADE IT!!
    // we are considering compression as in time instead of size, because it is less complicated than trying to 
    // calculate the size of a packet
    // delta high - delta low > threshold if it is above we have compression happening!

    if ((highEntropy - lowEntropy) > 100) {
        strncpy(result, "Compression Detected!", 26);
    } else {
        strncpy(result, "Compression Not Detected!", 26);
    }
    
    recv(socketFd, udpData, MAXDATA ,0);
    send(connectionFd, result, 26, 0);
    free(udpData);
    close(socketFd);
    close(connectionFd);
}

