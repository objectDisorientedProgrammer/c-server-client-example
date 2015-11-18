/*
	client.h
	Author: Douglas Chidester
	Date:	17 November 2015
	Version: Linux
*/
#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define PORT_NUM	71153
#define BUFFER_SIZE 32768
#define ERROR_VAL	-1
#define MIN_LENGTH	0

typedef struct sockaddr_in SAin;

#endif
