#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>


char *ip;
char *port;
char *pword;

void* client_1(void* data)        //part1
{
    
    int client_id=data;

    int l_minus=0;
    //socket的建立
    int sockfd = 0;
    //socket的連線
    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;
    //localhost test
    int portNum;
    char *ptr;

    portNum = strtol(port, &ptr,10);
    info.sin_addr.s_addr = inet_addr(ip);
    info.sin_port = htons(portNum);
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if(connect(sockfd,(struct sockaddr *)&info,sizeof(info))==-1)
    {
        
        printf("thread%dConnection error\n",client_id);

        fflush(stdout);
    }

    printf("client%d start order\n",client_id);
    char input_1[256]="confirm";
    send(sockfd,input_1,256,0);
    char rec[256]={};
    recv(sockfd,rec,sizeof(rec),0);
    if(strcmp(rec,"Please order some meals")!=0)
    {
        l_minus++;
        printf("********************\nyour commend: %s\nanswer: Please order some meals\n********************\n\n",rec);
        fflush(stdout);
    }
    
    char input_2[256]="order Egg-drop-soup 2";
    send(sockfd,input_2,256,0);
    recv(sockfd,rec,sizeof(rec),0);
    if(strcmp(rec,"Egg-drop-soup 2")!=0)
    {
        l_minus++;
        printf("********************\nyour commend: %s\nanswer: Egg-drop-soup 2\n********************\n\n",rec);
        fflush(stdout);
    }   
    //fflush(stdout);   
    char input_3[256]="confirm";
    send(sockfd,input_3,256,0);
    recv(sockfd,rec,sizeof(rec),0);
    if(strcmp(rec,"Please wait a few minutes...")==0)
    {
        recv(sockfd,rec,sizeof(rec),0);
        if(strcmp(rec,"Delivery has arrived and you need to pay 100$")!=0)
        {
            //time_arr[now_time]+=1;
            l_minus++;
            printf("********************\nyour commend: %s\nanswer: Delivery has arrived and you need to pay 100$\n********************\n\n",rec);
            fflush(stdout);
        }
        printf("client%d finish1\n",client_id);
        fflush(stdout);
        // sem_wait(&timearr_sem);
        // time_arr[now_time]+=1;
        // sem_post(&timearr_sem);
    }
    else if(strcmp(rec,"Your delivery will take a long time, do you want to wait?")==0)
    {
        printf("client%d finish0\n",client_id);
        fflush(stdout);
        char input_4[256]="No";
        send(sockfd,input_4,256,0);     
    }
    else
    {
        printf("********************\nyour commend: %s\nanswer: Please wait a few minutes... or Your delivery will take a long time, do you want to wait?\n********************\n\n",rec);
        fflush(stdout);
    }
    
    
    fflush(stdout);
    close(sockfd);
}

int main(int argc , char *argv[])
{
    // struct itimerval timer;
    // timer.it_value.tv_sec=0;
    // timer.it_value.tv_usec=100000;
    // timer.it_interval.tv_sec=0;
    // timer.it_interval.tv_usec=100000 ;
    // setitimer(ITIMER_REAL,&timer,NULL);
    // signal(SIGALRM,itimer);
    // sem_init(&timearr_sem,0,1);
    //####################################################################//

    int l_minus=0;
    //socket的建立
    int sockfd = 0;
    //socket的連線
    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;
    //localhost test
    int portNum;
    char *ptr;
    ip = argv[1];
    port = argv[2];
    if(argv[3]==NULL)
        pword="0";
    else 
        pword = argv[3];
    if(ip==NULL||port==NULL)
    {
        puts("格式: ./hw2_checker <ip> <port>");
        return -1;
    }

    portNum = strtol(port, &ptr,10);
    info.sin_addr.s_addr = inet_addr(ip);
    info.sin_port = htons(portNum);
    sockfd = socket(AF_INET , SOCK_STREAM , 0);


    pthread_t th[50];


    for(int i=0;i<32;i++)
    {
        pthread_create(&th[i], NULL, client_1, (void*)i);

    }

    for(int i=0;i<32;i++)
    {
        pthread_join(th[i], NULL);

    }

    puts("");








}