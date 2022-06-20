// server (process + share_memory + semaphore version)
#include <errno.h>
#include <fcntl.h>  //open (linux)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>  //read, write (linux)
#include <signal.h>
#include <sys/shm.h>  //share memory

#include "sockop.h"

#define BUFSIZE 50

#define SEM_MODE 0666
#define SEM_KEY 1122334455

#define SHMSZ 20

//semaphore
int sem;

// share memory
int shmid;
key_t key = 5678;  //we'll name our shared memory segment "5678"
int* remain_money; //shm (share memory segment)

/*P() return 0 if ok, -1 otherwise*/
/*
    P semaphore function signals that the task
    requires a resource and if not available
    waits for it. 
*/
int P(int s){  //常用於獲取資源的使用權
    struct sembuf sop; // the operation parameters
    sop.sem_num = 0;
    sop.sem_op = -1; //wait  //If sem_op is a positive integer, the operation adds this value to the semaphore value (semval). 
    sop.sem_flg = 0;

    if ( semop(s, &sop, 1) < 0){
        fprintf(stderr, "P():semop failed: %s\n", strerror(errno));
        return -1;
    }else{
        return 0;
    }
}

/*V() return 0 if ok, -1 otherwise*/
/*
    V semaphore function signals which the
    task passes to the OS that the resource is
    now free for the other users.
*/
int V(int s){
    struct sembuf sop; // the operation parameters
    sop.sem_num = 0;
    sop.sem_op =  1; //signal
    sop.sem_flg = 0;

    if (semop(s, &sop, 1) < 0){
        fprintf(stderr, "V():semop failed: %s\n", strerror(errno));
        return -1;
    }else{
        return 0;
    }
}

/*increment money in account*/
void increment(int amount, int times, int* remain_money)
{
    while(times)
    {
        /*acquire semaphore*/
        P(sem);
        /******************* Critical section **********************/
        // transfer string to integer & increment counter 
        *remain_money += amount;
        printf("After deposit: %d\n", *remain_money);
        /***********************************************************/
        //release semaphore
        V(sem);

        times--;
    }
}


/*increment money in account*/
void decrease(int amount, int times, int* remain_money)
{
    while(times)
    {
        /*acquire semaphore*/
        P(sem);
        /******************* Critical section **********************/
        // transfer string to integer & increment counter 
        *remain_money -= amount;
        printf("After withdraw: %d\n", *remain_money);
        /***********************************************************/
        //release semaphore
        V(sem);

        times--;
    }
}

//signal handler
// for cleaning zombie process
void clean_zp_handler(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void CtrlCHandler(int signum) {
    //remove semaphore
    if (semctl(sem, 0, IPC_RMID, 0) < 0)
    {
        fprintf(stderr, "Unable to remove semaphore: %1d\n", SEM_KEY);
        exit(1);
    }
    printf("Semaphore %1d has been remove\n", SEM_KEY);

    //Destroy the share memory segment
    printf("Server destroy the share memory.\n");
    int retval = shmctl(shmid, IPC_RMID, NULL);
    if (retval < 0)
    {
        fprintf(stderr, "Server remove share memory failed.\n");
        exit(1);
    }
}

int main(int argc , char *argv[])
{   
    pid_t childpid;

    int sockfd, connfd; //socket descriptor
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    int status;
    char rcv[BUFSIZE], operation[BUFSIZE];
    int amount, times;


    if (argc != 2)
        errexit("Usage: %s <port>\n", argv[0]);

    /*create socket and bind socket to port*/
    sockfd = passivesock(argv[1], "tcp", 10);

    //create the share memory segment
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }
    printf("create shmid : %d\n", shmid);

    //attach the segment and initial to zero
    if ((remain_money = shmat(shmid, NULL, 0)) == (int *) -1){
        perror("shmat");
        exit(1);
    }
    *remain_money = 0;


    /* create semaphore */
    sem = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
    if (sem < 0)
    {
        fprintf(stderr, "Creation of semaphore %1d failed: %s\n", SEM_KEY, strerror(errno));
        exit(-1);
    }
    
    // initial semaphore value to 1 (binary semaphore)
    if ( semctl(sem, 0, SETVAL, 1) < 0)
    {
        fprintf(stderr, "Unable to initialize semaphore: %s\n", strerror(errno));
        exit(0);
    }
    printf("Semaphore %1d has been created & initialized to 1\n", SEM_KEY);


    int cnt = 0;
    while (1){
        signal(SIGINT, CtrlCHandler);
        /* waiting for connection, accept connection from clients */
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if (connfd == -1)
            errexit("Error: accept()\n");

        // Print number of clients connected till now
        printf("# of Clients connected: %d\n",++cnt);


        childpid = fork();
        signal(SIGCHLD, clean_zp_handler);     // SIGCHLD: signal for Child stopped or terminated,  cleaning zombie process 但沒有好像也不會有zombie
        if (childpid >= 0){  //>0 fork succeeded parent,  =0 child
            if (childpid == 0){ 
                // in the child process
                close(sockfd);                // 在child process裡不會再用到了所以可以close, 但沒close也沒差，不會影響到其他人連，因為進到child process後就是自己一支的程式了

                // read message from client
                memset(rcv, 0, BUFSIZE);
                int n;
                if ((n = read(connfd, rcv, BUFSIZE)) == -1)
                    errexit("Error: read()\n");
                sscanf(rcv, "%s %d %d", operation, &amount, &times);    //從str指定的字符串讀取數據，並根據參數format字符串來轉換並格式化數據。
                

                //get the already created share memory segment
                if ((shmid = shmget(key, SHMSZ, 0666)) < 0)
                {
                    perror("shmget");
                    exit(1);
                }
                printf("in child process shmid is %d\n", shmid);


                //attach the segment to our data space
                if ((remain_money = shmat(shmid, NULL, 0)) == (int *) -1){
                    perror("shmat");
                    exit(1);
                }
                printf("in child process attach the share memory.\n");


                // do the operation
                if (!strcmp(operation,"deposit")){
                    increment(amount, times, remain_money);
                }
                else if (!strcmp(operation,"withdraw"))
                    decrease(amount, times, remain_money);
                else{
                    printf("operation read from client is incorrect.");
                    exit(1);
                }
                
                //Detach the share memory segment
                shmdt(remain_money);        

                exit(0);  //success            
            }
        }else{   //fork failed, childpid will = -1
            perror("fork");
            exit(1);
        }

        close(connfd);                //close client connection
    }

    return 0;
}