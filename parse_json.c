#include <errno.h>

#include "parse_json.h"


void read_json(struct json *json_s, char *config_file) {
    FILE *fptr = NULL;
    fptr = fopen(config_file, "r");
    char *buff;
    size_t buffer_size = 100;
    size_t line_len;

    while (true) {
        buff = (char *) malloc(buffer_size * sizeof(char));

        if(buff == NULL) {
            continue;
        }
        line_len = getline(&buff, &buffer_size, fptr);
        
        if (feof(fptr)) {
            free(buff);
            break;
        }
        
        load_json_s(json_s, buff);

        printf("line: %s\n", buff);
        free(buff);
    }
    fclose(fptr);
}

void load_json_s(struct json * json_s, char *line) {
    //check the line we get for the keywords in the config file
    
    if (strstr(line, "server ip address") != NULL) {
        json_s->serverIp = strdup(line); //allocate mem for the server ip address
        json_s->serverIp = json_s->serverIp + strcspn(json_s->serverIp, ":") + 3; //pointer magic
        *(json_s->serverIp + strcspn(json_s->serverIp, "\"")) = '\0'; //more pointer magic

    } else if (strstr(line, "src port number udp") != NULL) {
        json_s->srcPortUdp = strdup(line); //allocate mem for the server ip address
        json_s->srcPortUdp = json_s->srcPortUdp + strcspn(json_s->srcPortUdp, ":") + 3; //pointer magic
        *(json_s->srcPortUdp + strcspn(json_s->srcPortUdp, "\"")) = '\0'; //more pointer magic

    } else if (strstr(line, "dst port number udp") != NULL) {
        json_s->dstPortUdp = strdup(line); //allocate mem for the server ip address
        json_s->dstPortUdp = json_s->dstPortUdp + strcspn(json_s->dstPortUdp, ":") + 3; //pointer magic
        *(json_s->dstPortUdp + strcspn(json_s->dstPortUdp, "\"")) = '\0'; //more pointer magic

    } else if (strstr(line, "dst port number tcp head") != NULL) {
        json_s->dstPortTcpHead = strdup(line); //allocate mem for the server ip address
        json_s->dstPortTcpHead = json_s->dstPortTcpHead + strcspn(json_s->dstPortTcpHead, ":") + 3; //pointer magic
        *(json_s->dstPortTcpHead + strcspn(json_s->dstPortTcpHead, "\"")) = '\0'; //more pointer magic

    } else if (strstr(line, "dst port number tcp tail") != NULL) {
        json_s->dstPortTcpTail = strdup(line); //allocate mem for the server ip address
        json_s->dstPortTcpTail = json_s->dstPortTcpTail + strcspn(json_s->dstPortTcpTail, ":") + 3; //pointer magic
        *(json_s->dstPortTcpTail + strcspn(json_s->dstPortTcpTail, "\"")) = '\0'; //more pointer magic

    } else if (strstr(line, "tcp port number") != NULL) {
        json_s->tcpPort = strdup(line); //allocate mem for the server ip address
        json_s->tcpPort = json_s->tcpPort + strcspn(json_s->tcpPort, ":") + 3; //pointer magic
        *(json_s->tcpPort + strcspn(json_s->tcpPort, "\"")) = '\0'; //more pointer magic

    } else if (strstr(line, "payload size") != NULL) {
        char *tmp = strdup(line); //allocate mem for the server ip address
        tmp = tmp + strcspn(tmp, ":") + 3; //pointer magic
        *(tmp + strcspn(tmp, "\"")) = '\0'; //more pointer magic

        json_s->payloadSize = atoi(tmp);

        if (json_s->payloadSize <= 0) {
            json_s->payloadSize = 1000;
        }

    } else if (strstr(line, "inter-measurement time") != NULL) {
        char *tmp = strdup(line); //allocate mem for the server ip address
        tmp = tmp + strcspn(tmp, ":") + 3; //pointer magic
        *(tmp + strcspn(tmp, "\"")) = '\0'; //more pointer magic

        json_s->msrTime = atoi(tmp);

        if (json_s->msrTime <= 0) {
            json_s->msrTime = 15;
        }

    } else if (strstr(line, "udp num packets") != NULL) {
        char *tmp = strdup(line); //allocate mem for the server ip address
        tmp = tmp + strcspn(tmp, ":") + 3; //pointer magic
        *(tmp + strcspn(tmp, "\"")) = '\0'; //more pointer magic

        json_s->numPackets = atoi(tmp);

        if (json_s->numPackets <= 0) {
            json_s->numPackets = 6000;
        }

    } else if (strstr(line, "ttl") != NULL) {
        char *tmp = strdup(line); //allocate mem for the server ip address
        tmp = tmp + strcspn(tmp, ":") + 3; //pointer magic
        *(tmp + strcspn(tmp, "\"")) = '\0'; //more pointer magic

        json_s->ttl = atoi(tmp);

        if (json_s->ttl <= 0) {
            json_s->ttl = 255;
        }
    }

}

void print_json_s(struct json *json_s) {
    printf("Server IP: %s\n", json_s->serverIp);
    printf("Source Port UDP: %s\n", json_s->srcPortUdp);
    printf("Dest Port UDP: %s\n", json_s->dstPortUdp);
    printf("Dest Port TCP Head: %s\n", json_s->dstPortTcpHead);
    printf("Dest Port TCP Tail: %s\n", json_s->dstPortTcpTail);
    printf("TCP Port: %s\n", json_s->tcpPort);
    printf("Payload Size: %d\n", json_s->payloadSize);
    printf("Measure Time: %d\n", json_s->msrTime);
    printf("Number of Packets: %d\n", json_s->numPackets);
    printf("TTL: %d\n", json_s->ttl);
}
