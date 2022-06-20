#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>


#include "sockop.h"

#define BUFSIZE 256

// read std input from terminal (user input) and send back to server
void sendback_message_to_server(int connfd){
    char input[BUFSIZE];
    memset(input, 0, BUFSIZE);
    fgets(input, sizeof(input), stdin);  // read string
    input[strcspn(input, "\n")] = 0;  //important!!!   The function fgets might add a newline at the end of the string read
    
    //Send a message to server
    int wn;
    if ((wn = write(connfd, input, strlen(input))) == -1)
        errexit("Error: write()\n");
}


//receiveMessage from client: shop list, order, confirm, cancel, Yes/No
void receive_mssage_from_server(int connfd, char* massage){
    memset(massage, 0, BUFSIZE);
    int rn;
    if ((rn = read(connfd, massage, BUFSIZE)) == -1)
        errexit("Error: read()\n");
}

int main(int argc , char *argv[])
{

    //socket的建立
    int connfd = 0;  //socket descriptor
    int n, rn;
    char input[BUFSIZE];
    char receiveMessage[BUFSIZE];
    int distance;

    if (argc != 3)
        errexit("Usage: %s <ip> <port>\n", argv[0]);  //ex: ./hw3_Client 127.0.0.1 7000

    //socket的連線
    connfd = connectsock(argv[1] , argv[2] , "tcp");
    printf("client connect to %d connfd\n", connfd);

    void CtrlCHandler(int signum) {
        close(connfd);   //此client不再連線，並結束程式
        exit(0);
    }

    do{  
        signal(SIGINT, CtrlCHandler); //signal user press ctrl-C
        printf("Client input: ");
        
        sendback_message_to_server(connfd);
            
        receive_mssage_from_server(connfd, receiveMessage);

    }while(strcmp(receiveMessage, "Please wait a few minutes...") != 0);


    //after server send "Please wait a few minutes..."
    receive_mssage_from_server(connfd, receiveMessage);
    //It should be "Delivery has arrived and you need to pay ?$" / "Your delivery will take a long time, do you want to wait?"
    printf("receiveMessage is :\n%s\n", receiveMessage);  


    //server send "Your delivery will take a long time, do you want to wait?"
    if (strcmp(receiveMessage, "Your delivery will take a long time, do you want to wait?") != 0){    
        printf("Client input: ");    
        //Send a message to server
        sendback_message_to_server(connfd);

        receive_mssage_from_server(connfd, receiveMessage);
        printf("receiveMessage is :\n%s\n", receiveMessage);    //It should be "Delivery has arrived and you need to pay ?$"
    }


    close(connfd);

    return 0;
}

