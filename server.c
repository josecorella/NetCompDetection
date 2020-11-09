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

int set_up_server(int *socketFd, int sockType, struct sockaddr_in *server, struct sockaddr_in *client) {
    int connectionFd, clientSize;

    // open a socket
    *socketFd = socket(AF_INET, sockType, 0);
    if (*socketFd == -1) {
        fprintf(stderr, "Failed creating socket :(\n");
        exit(EXIT_FAILURE);
    }

    // set up server socket struct
    memset(&server, 0, sizeof(server));
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
    clientSize = sizeof(client);
    connectionFd = accept(*socketFd, (struct sockaddr *) client, &clientSize);
    if (connectionFd == -1) {
        fprintf(stderr, "Failed to receive data on socket: %d\n", *socketFd);
        exit(EXIT_FAILURE);
    }

    return connectionFd;
}

void send_response(int connection, char *data, int dataLen, struct json *tcpInfo) {
}

int main (int argc, char **argv) {
    int socketFd, connectionFd, dataLen;
    struct sockaddr_in serverAddress, client;
    struct json tcpInfo;
    char data[MAXDATA] = {0};

    /*Set up connection so that we get can listen */
    connectionFd = set_up_server(&socketFd, SOCK_STREAM, &serverAddress, &client);
    send_response(connectionFd, data, MAXDATA, &tcpInfo);
    

}

