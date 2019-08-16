/*
    client.c
    Author: Douglas Chidester
    
    Version: Linux
*/

#include "client.h"
#include "common.h"

#define MIN_LENGTH 0

void errorCloseSock(char* msg, int sock)
{
    close(sock);
    CMN_error(msg);
}

void printHelp()
{
    puts("Commands:");
    printf("  %s directory [directory ...] - List directory.\n", CMN_listCommand);
    printf("  %s - Quit client.\n", CMN_clientQuitCommand);
    printf("  %s - Quit server and client.\n", CMN_endSessionCommand);
    printf("  %s - Display command options.\n", CMN_helpCommand);
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
        CMN_error("Socket create error.");
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
    printf("\nEnter a command ('%s' for options):\n", CMN_helpCommand);
    do
    {
        getUserInput(buf);
        if(strcmp(buf, CMN_helpCommand) == 0) // check for 'help' command
        {
            printHelp();
        }
        // check for quit command
        else if(strcmp(buf, CMN_clientQuitCommand) == 0
                || strcmp(buf, CMN_endSessionCommand) == 0)
        {
            done = 1;
            strncpy(sendBuf, CMN_endSessionCommand, sizeof(CMN_endSessionCommand));
            
            // TODO fix this duplication below
            // send data to server
            sendMessage(sockfd, sendBuf, strlen(sendBuf));
            memset(sendBuf, 0, strlen(sendBuf));
            // get data from server
            getResponse(sendBuf, buf, sockfd);
        }
        else
        {
            strcpy(sendBuf, buf);
            // TODO fix this duplication above
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
