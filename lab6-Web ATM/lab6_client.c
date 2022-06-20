#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "sockop.h"

#define BUFSIZE 50

int main(int argc , char *argv[])
{

    //socket的建立
    int connfd = 0;  //socket descriptor
    int n;
    char msg[BUFSIZE];

    if (argc != 6)
        errexit("Usage: %s <ip> <port> <deposit/withdraw> <amount> <times>\n", argv[0]);

    //socket的連線
    connfd = connectsock(argv[1] , argv[2] , "tcp") ;

    //write message to server
    sprintf(msg, "%s %s %s", argv[3], argv[4], argv[5]);
    if ((n = write(connfd, msg, strlen(msg))) == -1)
        errexit("Error: write()\n");
    

    close(connfd);
    return 0;
}