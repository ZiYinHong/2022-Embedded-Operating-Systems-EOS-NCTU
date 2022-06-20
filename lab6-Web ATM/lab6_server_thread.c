// server (thread(so no need to use share memory) + mutex version)
#include <errno.h>
#include <fcntl.h>  //open (linux)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>  //share memory
#include <sys/sem.h>  //semaphore
#include <sys/stat.h>
#include <sys/types.h>  //share memory
#include <sys/wait.h>
#include <unistd.h>  //read, write (linux)
#include <signal.h>
#include <pthread.h>

#include "sockop.h"

#define BUFSIZE 50
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#define NUMTHREADS 4
int remain_money = 0;

#define checkResults(string, val){\
    if (val){                     \
        printf("Failed with %d at %s\n", val, string); \
        exit(1); \
    }\
}

/*increment and decrease money in account*/
void *thread_lock_mutex(void * receive_msg)  //return function pointer
{   
    char operation[BUFSIZE];
    int amount, times;
    sscanf(receive_msg, "%s %d %d", operation, &amount, &times);

    int rc;
    printf("\tThread %lu: Entered\n", (unsigned long)pthread_self());

    //lock mutex
    rc = pthread_mutex_lock(&mutex);
    checkResults("pthread_mutex_lock()\n", rc);
    printf("\tThread %lu: Start critical section, holding lock\n", (unsigned long)pthread_self());

    /******************* Critical section **********************/

    // Access to shared data goes here
    if (!strcmp(operation,"deposit"))
        for (int i = 0; i < times; i++){
            remain_money += amount;
            printf("After deposit: %d\n", remain_money);
        }
    else if (!strcmp(operation,"withdraw"))
        for (int i = 0; i < times; i++){
            remain_money -= amount;
            printf("After withdraw: %d\n", remain_money);
        }
    else{
        printf("operation read from client is incorrect.");
        exit(1);
    }

    /************************************************************/
    
    //unlock mutex
    rc = pthread_mutex_unlock(&mutex);
    checkResults("pthread_mutex_unlock()\n", rc);
    printf("\tThread %lu: End critical section, release lock\n", (unsigned long)pthread_self());


    return NULL;
}



int main(int argc , char *argv[])
{   
    pid_t childpid;
    int sockfd, connfd; //socket descriptor
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    int n, status;
    char rcv[BUFSIZE];
    int rc = 0;


    if (argc != 2)
        errexit("Usage: %s <port>\n", argv[0]);

    /*create socket and bind socket to port*/
    sockfd = passivesock(argv[1], "tcp", 10);

    //initial mutex
    pthread_mutex_init(&mutex, NULL);

    int thread_nums = 0;
    pthread_t   thread[NUMTHREADS];       //pthread_t* thread; //create thread
    while (1){
        /* waiting for connection, accept connection from clients */
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if (connfd == -1)
            errexit("Error: accept()\n");

        //signal(SIGCHLD, handler);
        
        // read message from client
        memset(rcv, 0, BUFSIZE);
        if ((n = read(connfd, rcv, BUFSIZE)) == -1)
            errexit("Error: read()\n");


        printf("Main thread create/start threads\n");
        rc = pthread_create(&thread[thread_nums++], NULL, thread_lock_mutex, rcv); //&(thread[thread_nums++]) 問
        checkResults("pthread_create()\n", rc);

        // wait thread complete
        if (thread_nums == NUMTHREADS){
            sleep(1); //Main thread wait a bit until all threads were done
            for(int i = 0; i < NUMTHREADS; i++){
                printf("pthread_join(thread[%d])\n", i);
                rc = pthread_join(thread[i], NULL);
                checkResults("pthread_join()\n", rc);
            }
            break; //get out of while(1)
        }
    }

    /*close client connection*/
    close(connfd);

    //destroy mutex
    printf("Main thread clean up mutex\n");
    rc = pthread_mutex_destroy(&mutex);
    printf("Main thread completed\n");

    return 0;
}