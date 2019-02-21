/**
    This header file contains all common includes between client and server.
    
    ClientServer_Common
 */
#ifndef CS_COMMON_H
#define CS_COMMON_H

#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


#define DEFAULT_PORT_NUM  71153
#define BUFFER_SIZE  32768

typedef struct sockaddr_in SAin;

#endif // CS_COMMON_H

