/*
	client.c
	Author: Douglas Chidester
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define PORT 71153
#define SIZE 32768

void error(char* msg)
{
	perror(msg);
	exit(-1);
}

void errorCloseSock(char* msg, int sock)
{
	close(sock);
	error(msg);
}

void printHelp()
{
	puts("Commands:");
	puts("  list <DIRECT OR FILE> [DIRECT OR FILE ...] - List dir or file.");
	puts("  done - Quit client.");
	puts("  kill - Quit server and client.");
	puts("  help - Display command options.");
}

int sendMessage(int fd, char* buf, int expLen)
{
	int len = write(fd, buf, strlen(buf));
	if(len < 0)
		errorCloseSock("send error", fd);
	memset(buf, 0, strlen(buf));
	return len;
}

void setupSocket(SAin* addr)
{
	memset(addr, 0, sizeof(addr));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	addr->sin_port = htons(PORT);
}

int createSocket(int l, SAin* adrs)
{
	l = socket(AF_INET, SOCK_STREAM, 0);
	if(l < 0)
		error("socket error");
	setupSocket(adrs);	// init socket addr
	return l;
}

void getUserInput(char* buf)
{
	printf("$ ");
	fgets(buf, SIZE, stdin);
	buf[strlen(buf)-1] = '\0';
}

void getResponse(char* sendBuf, char* buf, int sockfd)
{
	int len = read(sockfd, sendBuf, SIZE);
	puts(sendBuf);
	memset(sendBuf, 0, SIZE);
	memset(buf, 0, SIZE);
}

// EXCEEDS 5 LINES
void clientLoop(int sockfd)
{
	int done = 0;
	char buf[SIZE], sendBuf[SIZE];
	puts("\nEnter a command ('help' for options):");
	do
	{
		getUserInput(buf);
		if(strcmp(buf, "help") == 0)	// check for 'help' command
		{
			printHelp();
			continue;
		}
		// check for quit command
		if(strcmp(buf, "done") == 0 || strcmp(buf, "kill") == 0)
			done = 1;
		
		strcpy(sendBuf, buf);
		// send data to server
		sendMessage(sockfd, sendBuf, strlen(sendBuf));
		memset(sendBuf, 0, strlen(sendBuf));
		// get data from server
		getResponse(sendBuf, buf, sockfd);	
	} while(!done);
}

int main(int argc, char* argv[])
{
	int rcv, sockfd;
	sockaddr_in addr;
	
	// create socket
	sockfd = createSocket(sockfd, &addr);
	// connect to server
	rcv = connect(sockfd, (struct sockaddr*) &addr, sizeof(sockaddr_in));
	if(rcv < 0)
		errorCloseSock("connect error", sockfd);
	
	// start main loop
	clientLoop(sockfd);
	
	close(sockfd);
    return 0;
}
