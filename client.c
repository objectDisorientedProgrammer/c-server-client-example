/*
    client.c
    Author: Douglas Chidester
    
    Version: Linux
*/

#include "client.h"

void error(char* msg)
{
    perror(msg);
    exit(ERROR_VAL);
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
    if(len < MIN_LENGTH)
        errorCloseSock("Send message error.", fd);
    memset(buf, 0, strlen(buf));
    return len;
}

void setupSocket(SAin* addr)
{
    memset(addr, 0, sizeof(addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    addr->sin_port = htons(DEFAULT_PORT_NUM);
}

int createSocket(int l, SAin* adrs)
{
    l = socket(AF_INET, SOCK_STREAM, 0);
    if(l < MIN_LENGTH)
        error("Socket create error.");
    setupSocket(adrs);    // init socket addr
    return l;
}

void getUserInput(char* buf)
{
    printf("$ ");
    fgets(buf, BUFFER_SIZE, stdin);
    buf[strlen(buf)-1] = '\0'; // add null terminator
}

void getResponse(char* sendBuf, char* buf, int sockfd)
{
    int len = read(sockfd, sendBuf, BUFFER_SIZE);
    puts(sendBuf);
    memset(sendBuf, 0, BUFFER_SIZE);
    memset(buf, 0, BUFFER_SIZE);
}

// EXCEEDS 5 LINES
void clientLoop(int sockfd)
{
    char done = 0;
    char buf[BUFFER_SIZE], sendBuf[BUFFER_SIZE];
    puts("\nEnter a command ('help' for options):");
    do
    {
        getUserInput(buf);
        if(strcmp(buf, "help") == 0)    // check for 'help' command
        {
            printHelp();
            //continue; // TODO remove this and change to if/else
        }
        // check for quit command
        else if(strcmp(buf, "done") == 0 || strcmp(buf, "kill") == 0)
            done = 1;
        else
        {
            strcpy(sendBuf, buf);
            // send data to server
            sendMessage(sockfd, sendBuf, strlen(sendBuf));
            memset(sendBuf, 0, strlen(sendBuf));
            // get data from server
            getResponse(sendBuf, buf, sockfd);
        }
    } while(!done);
}

int main(int argc, char* argv[])
{
    int rcv, sockfd;
    SAin addr;
    
    // TODO check cmd line arguments
    
    // create socket
    sockfd = createSocket(sockfd, &addr);
    // connect to server
    rcv = connect(sockfd, (struct sockaddr*) &addr, sizeof(SAin));
    if(rcv < 0)
        errorCloseSock("Connection error. Is the server running?", sockfd);
    
    // start main loop
    clientLoop(sockfd);
    
    close(sockfd);
    return 0;
}
