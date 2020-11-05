#ifndef _RD_JN_
#define _RD_JN_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct json {
    char *serverIp;
    char *srcPortUdp;
    char *dstPortUdp;
    char *dstPortTcpHead;
    char *dstPortTcpTail;
    char *tcpPort;
    int payloadSize;
    int msrTime;
    int numPackets;
    int ttl;
};


void read_json(struct json * json_s, char *config_file);
void load_json_s(struct json * json_s, char *line);
void print_json_s(struct json * json_s);

#endif
