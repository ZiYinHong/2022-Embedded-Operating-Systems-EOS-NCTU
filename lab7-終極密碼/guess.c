#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/time.h>
#define errexit(format,arg...) exit(printf(format,##arg))

// struct want to store in share memory;
typedef struct {
    int guess;
    char result[8];
}data;  
data shm_data;  //直接用data也可

//define share memory size
#define SHMSZ sizeof(shm_data)  //sizeof(data)
// variables for share memory
int shmid;
int key_int;
data *shm;

//upper_bound
int upper_bound;
int lower_bound = 1;
//game program PID
int game_pid;

void SIGUSR1_handler(int signo, siginfo_t *info, void *context){
    if(strcmp(shm->result , "larger") == 0){
        lower_bound = shm->guess;
    }else if(strcmp(shm->result , "smaller") == 0){
        upper_bound = shm->guess;
    }else{
        exit(0); //bingo! program end
    }       
}


void timer_handler(int signum)
{   
    shm->guess = (upper_bound + lower_bound)/2;
    printf("[game] Guess: %d\n", shm->guess);
    kill(game_pid, SIGUSR1);
}


int main (int argc, char *argv[]){

    if (argc != 4)
        errexit("Usage: %s <key> <upper_bound> <pid>\n", argv[0]);

    sscanf(argv[2], "%d", &upper_bound);
    sscanf(argv[3], "%d", &game_pid);
    pid_t pid = game_pid;

    //variables for SIGUSR1, and timer
    struct sigaction SIGUSR1_sa, timer_sa;
    //variables for timer
    struct itimerval timer;  
    //variables for nanosleep
    struct timespec req;
    int retval;
    // variables for share memory
    sscanf(argv[1], "%d", &key_int);
    key_t key = key_int;  //<key> 為 shared memory 的識別 key


    //locate the share memory segment
    shmid = shmget(key, SHMSZ, 0666);
    shm = shmat(shmid, NULL, 0);

    
    //register handler to SIGUSR1
    memset(&SIGUSR1_sa, 0 ,sizeof(SIGUSR1_sa));
    SIGUSR1_sa.sa_flags = SA_SIGINFO;
    SIGUSR1_sa.sa_sigaction = SIGUSR1_handler;
    sigaction(SIGUSR1, &SIGUSR1_sa, NULL);
    
    //每一秒猜一次
    //install timer_handler as the signal handler for SIGALRM
    memset(&timer_sa, 0, sizeof(timer_sa));
    timer_sa.sa_sigaction = &timer_handler;
    sigaction(SIGVTALRM, &timer_sa, NULL); 
    //configure the timer to expire after 1 sec
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 100000;
    //reset the timer back to 1 sec after expired
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 100000;
    //Start a virtual timer
    setitimer(ITIMER_VIRTUAL, &timer, NULL);

    while(1);


    return 0;
}


