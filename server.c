/*
    server.c
    Author: Douglas Chidester
    
    Version: Linux
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/wait.h>

#define PORT 71153
#define SIZE 32768
#define MAX_CONNECTIONS 5

typedef struct sockaddr_in SAin;

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

void errorClose2Sock(char* msg, int sock, int sock2)
{
    close(sock);
    close(sock2);
    error(msg);
}

int createSocket(int l)
{
    l = socket(AF_INET, SOCK_STREAM, 0);
    if(l < 0)
        error("socket error");
    return l;
}

void setSockIn(SAin* addr)
{
    memset(addr, 0, sizeof(addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    addr->sin_port = htons(PORT);
}

int bindSocket(SAin addr, int sockfd)
{
    setSockIn(&addr);
    int rcv = bind(sockfd, (struct sockaddr*) &addr, sizeof(addr));
    if(rcv < 0)
        errorCloseSock("bind error", sockfd);
    return rcv;
}

int listenForClient(int lSock)
{
    int get = listen(lSock, MAX_CONNECTIONS);
    if(get < 0)
        errorCloseSock("listen error", lSock);
    return get;
}

int acceptConnection(int socket)
{
    puts("Server listening - send 'kill' to quit"); // TODO
    int fd = accept(socket, NULL, NULL);
    if(fd < 0)
        errorCloseSock("accept error", socket);
    return fd;
}

int getMessage(int aSock, int lSock, char* msg)
{
    char buffer[SIZE];
    int bits = read(aSock, buffer, SIZE);
    printf("got: '%s'\n", buffer);
    if(bits < 0)
        errorClose2Sock("recv error", aSock, lSock);
    strcpy(msg, buffer);
    memset(buffer, 0, SIZE);
    return bits;
}

int sendMessage(int aSock, int lSock, char* msg)
{
    int rcv = write(aSock, msg, strlen(msg));
    if(rcv < 0)
        errorClose2Sock("send error", aSock, lSock);
    memset(msg, 0, strlen(msg));
    return rcv;
}

void setupArg(char** arg, char* file)
{
    arg[0] = "/bin/ls";
    arg[1] = "-l";
    arg[2] = file;
    arg[3] = NULL;
}

void createChild(char** arg, int s1, int s2)
{
    char* en[] = { "/bin", NULL};
    int old = s1;
    switch(fork())
    {
        case -1: errorClose2Sock("fork failed", s1, s2); break;
        case 0:
            dup2(s1, 1);
            if(execvp(arg[0], arg) == -1)
                errorClose2Sock("exec error", s1, s2);
            //fflush(1);
            
            //dup2(s1, old);
            //sendMessage(old, s2, "END");
            //exit(0);
        default:
            if(wait(NULL) == -1)
                errorClose2Sock("wait error", s1, s2);
    }
}

void doLsl(char* file, int s1, int s2)
{
    char* arg[4];
    setupArg(arg, file);
    createChild(arg, s1, s2);
    
}

void listContents(char* buf, int acpt, int lis)
{
    char* pch = strtok(buf, " ");
    char temp[SIZE];
    
    pch = strtok(NULL, " ");
    while(pch != NULL)
    {
        strcpy(temp, pch);
        // run ls -l and dup2 results to acpt
        doLsl(temp, acpt, lis);
        pch = strtok(NULL, " ");
        memset(temp, 0, sizeof(temp));
    }
    memset(buf, 0, sizeof(buf));
}

// TODO clean this up
int handleConnection(char* buf, int acpt, int lis, int rcv)
{
    puts("client connected.");
    while(strcmp(buf, "done") != 0)
    {
        // get message from client
        rcv = getMessage(acpt, lis, buf);
        //printf("client sent: '%s'\n", buf);
    
        if(strcmp(buf, "kill") == 0)    // exit
        {
            char gb[9];
            
            //sendMessage(acpt, lis, "Goodbye!");
            //puts("client disconnected");
            return 1;
        }
        else if(strstr(buf, "list") != NULL) // list dir contents
            listContents(buf, acpt, lis);
        else
            sendMessage(acpt, lis, buf);    // echo message back
        memset(buf, 0, strlen(buf));
    }
}

void loopOverClients(int lis, int rcv)
{
    int done = 0;
    int acpt;
    char buf[SIZE];
    
    while(!done)
    {    
        rcv = listenForClient(lis);
        acpt = acceptConnection(lis);
        // loop once per connection
        done = handleConnection(buf, acpt, lis, rcv);
        //close(acpt);
        memset(buf, 0, SIZE);
    }
    close(acpt);
}

int main(int argc, char* argv[])
{
    int lis, rcv;
    SAin addr;
    
    lis = createSocket(lis);
    rcv = bindSocket(addr, lis);
    // wait for incoming connection
    loopOverClients(lis, rcv);
    
    puts("Closing connections and exiting normally...");
    close(lis);
    return 0;
}
