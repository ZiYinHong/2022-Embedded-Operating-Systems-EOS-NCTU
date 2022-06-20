#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


#include "sockop.h"

#define BUFSIZE 256

int main(int argc , char *argv[])
{

    //socket的建立
    int connfd = 0;  //socket descriptor
    int n, rn;
    char input[BUFSIZE];
    char receiveMessage[BUFSIZE];
    int distance;

    if (argc != 3)
        errexit("Usage: %s <ip> <port>\n", argv[0]);

    //socket的連線
    connfd = connectsock(argv[1] , argv[2] , "tcp");

    void CtrlCHandler(int signum) {
        close(connfd);   //此client不再連線，並結束程式
        exit(0);
    }

    do{  
        signal(SIGINT, CtrlCHandler); //signal user press ctrl-C
        printf("Client input: ");
         
        fgets(input, sizeof(input), stdin);  // read string
        input[strcspn(input, "\n")] = 0;  //important!!!   The function fgets might add a newline at the end of the string read
        
        //Send a message to server
        if ((n = write(connfd, input, strlen(input))) == -1)
            errexit("Error: write()\n");
            
        memset(receiveMessage, 0, BUFSIZE);
        if ((rn = read(connfd, receiveMessage, BUFSIZE)) == -1)
            errexit("Error: read()\n");
        printf("receiveMessage is :\n%s\n", receiveMessage);

    }while(strcmp(receiveMessage, "Please wait a few minutes...") != 0);

    //Delivery has arrived and you need to pay %d$", total_money
    memset(receiveMessage, 0, BUFSIZE);
    if ((rn = read(connfd, receiveMessage, BUFSIZE)) == -1)  
        errexit("Error: read()\n");
    printf("receiveMessage is :\n%s\n", receiveMessage);

    close(connfd);

    return 0;
}

