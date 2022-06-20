// server 
#include <stdio.h>   //perror()
#include <stdlib.h>  //exit()
#include <string.h>
#include <unistd.h>  //dup2(), exec..()

#include <signal.h>
#include <sys/wait.h>

#include "sockop.h"

// for cleaning zombie process
void handler(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc , char *argv[])
{   
    pid_t childpid, pid;
    int sockfd, connfd; //socket descriptor
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    int n;
    int status;
    // char snd[BUFSIZE], rcv[BUFSIZE];

    if (argc != 2)
        errexit("Usage: %s <PORT>\n", argv[0]);

    /*create socket and bind socket to port*/
    sockfd = passivesock(argv[1], "tcp", 10);

    while (1){
        /* waiting for connection, accept connection from clients */
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if (connfd == -1)
            errexit("Error: accept()\n");
        
        childpid = fork();
        signal(SIGCHLD, handler);
        if (childpid >= 0){  //fork succeeded
            if (childpid == 0){ 
                // in the child process

                close(sockfd);                // impportant! Closing the server socket id
                printf("train ID : %d\n", getpid());  

                dup2(connfd, STDOUT_FILENO);  // duplicate socket on stdout             
                close(connfd);
                execlp("sl", "sl", "-l", NULL);
                /*we should not return from execlp, so only get to this line if it failed*/
                perror("exec");
                exit(1);            
            }
        }else{   //fork failed, childpid will = -1
            perror("fork");
            exit(1);
        }
    
        /*close client connection*/
        close(connfd);
    }

    return 0;
}