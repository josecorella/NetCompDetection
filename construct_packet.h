#ifndef _CNST_PKT_H_
#define _CNST_PKT_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
//#include <linux/if_ether.h>
// struct tcphdr
#include <netinet/tcp.h>
// struct udphdr
#include <netinet/udp.h>  
#include <sys/ioctl.h> 
#include <netinet/ip.h>
#include <arpa/inet.h> 
#include <errno.h>
//#include <linux/if_packet.h>
#include <net/if.h>
#include <time.h>
#include <pcap.h>
#include <sys/wait.h>

void entropy(uint8_t *data, int len);

#endif
