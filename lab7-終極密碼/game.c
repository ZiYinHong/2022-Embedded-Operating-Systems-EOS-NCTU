#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <time.h>
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
data* shm;

int guess_num;  // do not have read/write, so i think it don't need to use sig_atomic_t

//receive SIGUSR1 signal from guess program
//and compare guess_num with guess in share memory(update by guess program)
//store the comparison result in result(larger太大、smaller太小、equal猜中）
//and send back SIGUSR1 signal to guess program representing compare completion.最後發送 SIGUSR1 信號給 Guess 程式表示判斷完成。
void SIGUSR1_handler(int signo, siginfo_t *info, void *context){
    if(shm->guess < guess_num){
        //char str[8] = "larger";
        strcpy(shm->result, "larger");  //  Q
        printf("[game] Guess %d, %s\n", shm->guess, shm->result);
    }else if(shm->guess > guess_num){
        strcpy(shm->result, "smaller");
        printf("[game] Guess %d, %s\n", shm->guess, shm->result);
    }else{ 
        strcpy(shm->result, "equal");
        printf("[game] Guess %d, bingo\n", shm->guess);
    }       
    kill(info->si_pid, signo);  //int kill(pid_t pid, int sig)
}


//在 Game 程式結束時 (Ctrl + C)，將建立出來的 shared memory 給清除。
void CtrlCHandler(int signo, siginfo_t *info, void *context) {
    //Detach the share memory segment
    shmdt(shm);  
    //Destroy the share memory segment
    printf("game program destroy share memory.\n");
    int retval = shmctl(shmid, IPC_RMID, NULL);
    if (retval < 0)
    {
        fprintf(stderr, "gamer CtrlCHandler handler remove share memory failed.\n");
        exit(1);
    }
    exit(0);
}

int main(int argc , char *argv[])
{   
    if (argc != 3)
        errexit("Usage: %s <key> <guess>\n", argv[0]);

    pid_t pid = getpid();
    printf("[game] Game PID: %d\n", pid);
    //variables for SIGUSR1, and SIGINT(Ctrl-C)
    struct sigaction SIGUSR1_sa, CtrlC_sa;
    //variables for nanosleep
    struct timespec req;
    int retval;


    //store share memory key from user input <key>
    sscanf(argv[1], "%d", &key_int); // Q
    key_t key = key_int;  //<key> 為 shared memory 的識別 key
    

    //store user input <guess> in share memory's guess area
    //create the share memory segment
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0)
    {
        perror("shmget");
        exit(1);
    }
    printf("create shmid : %d\n", shmid);
    //attach the segment and initial to zero
    if ((shm = shmat(shmid, NULL, 0)) == (data *) -1){
        perror("shmat");
        exit(1);
    }
    sscanf(argv[2], "%d", &guess_num); // Q


    //set the sleep time to 60 secs
    memset(&req, 0, sizeof(struct timespec));
    req.tv_sec = 60;  
    req.tv_nsec = 0;
    
    //register handler to SIGUSR1
    //will receive SIGUSR1 signal from guess program
    memset(&SIGUSR1_sa, 0 ,sizeof(SIGUSR1_sa));
    SIGUSR1_sa.sa_flags = SA_SIGINFO;
    SIGUSR1_sa.sa_sigaction = SIGUSR1_handler;  
    sigaction(SIGUSR1, &SIGUSR1_sa, NULL);

    //register handler to CtrlC_sa
    //will receive SIGINT signal from guess program
    memset(&CtrlC_sa, 0 ,sizeof(CtrlC_sa));
    CtrlC_sa.sa_flags = SA_SIGINFO;
    CtrlC_sa.sa_sigaction = CtrlCHandler;  
    sigaction(SIGINT, &CtrlC_sa, NULL);
   
    //sleep 60 secs
    do{
        retval = nanosleep(&req, &req);
    }while(retval);

    return 0;
}
