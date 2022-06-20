#pragma once
#include <stdio.h>   //perror()
#include <stdlib.h>  //exit()
#include <string.h>
#include <unistd.h>  //dup2(), exec..(), read(), write()
#include <signal.h>
#include <sys/sem.h>  //semaphore
#include <sys/shm.h>  //share memory
#include <sys/ipc.h>    //share memory
#include <sys/types.h>  //share memory
#include <pthread.h>
#include "sockop.h"

#define checkResults(string, val){\
    if (val){                     \
        printf("Failed with %d at %s\n", val, string); \
        exit(1); \
    }\
}

#define errexit(format,arg...) exit(printf(format,##arg))
#define BUFSIZE 256


#define NUMTHREADS 40

//define struct for each delivery_man
//Any “leftover” members are given 0 as their initial value.//
// typedef struct {   //typedef in <stdio.h>
//     int delivery_time;  //delivery_time = delivery_distance
//     int income;
//     int customer;
//     int connfd;
// }delivery_man;  

// delivery_man delivery_man_1;
// delivery_man delivery_man_2;
int delivery_man_1_income = 0;
int delivery_man_2_income = 0;
int delivery_man_1_customer = 0;
int delivery_man_2_customer = 0;
int delivery_man_1_delivery_time = 0;
int delivery_man_2_delivery_time = 0;

// global variable
enum shop{Dessert, Beverage, Diner}; 


pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;

// return this meal oredered belong to which shop
int meal_to_shop(char* meal){
    if(strcmp(meal, "cookie") == 0 || strcmp(meal, "cake") == 0)
        return Dessert;
    else if(strcmp(meal, "tea") == 0 || strcmp(meal, "boba") == 0)
        return Beverage;
    else if(strcmp(meal, "fried-rice") == 0 || strcmp(meal, "Egg-drop-soup") == 0)
        return Diner;
 }

// deal with money, return order's money
int Dessertshop_order(char* meal, int amount){
    if(strcmp(meal, "cookie") == 0){
        return 60* amount;
    }else{
        return 80* amount;
    }
}

int Beverageshop_order(char* meal, int amount){
    if(strcmp(meal, "tea") == 0){
        return 40* amount;
    }else{
        return 70* amount;
    }
}

int Dinershop_order(char* meal, int amount){
    if(strcmp(meal, "fried-rice") == 0){
        return 120* amount;
    }else{
        return 50* amount;
    }
}

//以下三者基本一樣，為了印出的商品不同(且因順序需固定)而寫三個function
//在同一shop的情況下，再order新東西，需判斷order的是不是同一物品(若是，amount需相加,品相不新增/不是，則新增品項跟amount)
char* produce_new_ordered_Dessert(char* ordered, char* meal, int amount){
    char* temp = malloc(BUFSIZE);  //cause we need to return string(char*)
    char ordered_meal1[15];
    int ordered_amount1, ordered_amount2;
    int two_order_flag = 0;
    
    /* 需判斷order的是不是同一物品)*/
    //order 會有兩種長相 : 1.cake 1  2.cookie 3|cake 1
    for (int i = 0; i < strlen(ordered); i++){
        if (ordered[i] == '|'){
            two_order_flag = 1;
            break; 
        }
    }

    if (two_order_flag){  //2. cookie 3|cake 1
        sscanf(ordered, "%*s %d|%*s %d", &ordered_amount1, &ordered_amount2);   //*: An optional starting asterisk indicates that the data is to be read from the stream but ignored 
        if (strcmp("cookie", meal) == 0){  //品項1相同，增加品項1 amount，品項2不變
            amount += ordered_amount1;
            sprintf(temp, "%s %d|%s %d", "cookie", amount, "cake", ordered_amount2);  
        }else{                                  //品項2相同，增加品項2 amount，品項1不變
            amount += ordered_amount2;
            sprintf(temp, "%s %d|%s %d", "cookie", ordered_amount1, "cake", amount); 
        }
    }
    else{  //1. cake 1 or cookie 1
        sscanf(ordered, "%s %d", ordered_meal1, &ordered_amount1);
        if (strcmp(ordered_meal1, meal) == 0){  //品項相同，僅增加amount
            amount += ordered_amount1;
            sprintf(temp, "%s %d", meal, amount);  
        }else{                                  //品項不同，新增品項跟amount進ordered string
            if (strcmp("cookie", meal) == 0){   // meal = "cookie", ordered_meal1 ="cake"
                sprintf(temp, "%s %d|%s %d", "cookie", amount, "cake", ordered_amount1);  
            }else{                              // meal = "cake", ordered_meal1 ="cookie"
                sprintf(temp, "%s %d|%s %d", "cookie", ordered_amount1, "cake", amount); 
            }
        }
    }
    return temp;
}


char* produce_new_ordered_Beverage(char* ordered, char* meal, int amount){
    char* temp = malloc(BUFSIZE);  //cause we need to return string(which is *char)
    char ordered_meal1[15];
    int ordered_amount1, ordered_amount2;
    int two_order_flag = 0;
    
    /* 需判斷order的是不是同一物品)*/
    //order 會有兩種長相 : 1.tea 1  2.tea 3|boba 1
    for (int i = 0; i < strlen(ordered); i++){
        if (ordered[i] == '|'){
            two_order_flag = 1;
            break; 
        }
    }

    if (two_order_flag){  //2. tea 3|boba 1
        sscanf(ordered, "%*s %d|%*s %d", &ordered_amount1, &ordered_amount2);   
        if (strcmp("tea", meal) == 0){  //品項1相同，增加品項1amount，品項2不變
            amount += ordered_amount1;
            sprintf(temp, "%s %d|%s %d", "tea", amount, "boba", ordered_amount2);  
        }else{                                  //品項2相同，增加品項2amount，品項1不變
            amount += ordered_amount2;
            sprintf(temp, "%s %d|%s %d", "tea", ordered_amount1, "boba", amount); 
        }
    }
    else{  //1. tea 1 or boba 1
        sscanf(ordered, "%s %d", ordered_meal1, &ordered_amount1);
        if (strcmp(ordered_meal1, meal) == 0){  //品項相同，僅增加amount
            amount += ordered_amount1;
            sprintf(temp, "%s %d", meal, amount);  
        }else{                                  //品項不同，新增品項跟amount進ordered string
            if (strcmp("tea", meal) == 0){   // meal = "tea", ordered_meal1 ="boba"
                sprintf(temp, "%s %d|%s %d", "tea", amount, "boba", ordered_amount1);  
            }else{                           // meal = "boba", ordered_meal1 ="tea"
                sprintf(temp, "%s %d|%s %d", "tea", ordered_amount1, "boba", amount); 
            }
        }
    }
    return temp;
}


char* produce_new_ordered_Diner(char* ordered, char* meal, int amount){
    char* temp = malloc(BUFSIZE);  //cause we need to return string(char*)
    char ordered_meal1[15];
    int ordered_amount1, ordered_amount2;
    int two_order_flag = 0;
    
    /* 需判斷order的是不是同一物品)*/
    //order 會有兩種長相 : 1.fried-rice 1  2.fried-rice 3|Egg-drop-soup 1
    for (int i = 0; i < strlen(ordered); i++){
        if (ordered[i] == '|'){
            two_order_flag = 1;
            break; 
        }
    }

    if (two_order_flag){  //2. fried-rice 3|Egg-drop-soup 1
        sscanf(ordered, "%*s %d|%*s %d", &ordered_amount1, &ordered_amount2);   //*: An optional starting asterisk indicates that the data is to be read from the stream but ignored 
        if (strcmp("fried-rice", meal) == 0){  //品項1相同，增加品項1amount，品項2不變
            amount += ordered_amount1;
            sprintf(temp, "%s %d|%s %d", "fried-rice", amount, "Egg-drop-soup", ordered_amount2);  
        }else{                                  //品項2相同，增加品項2amount，品項1不變
            amount += ordered_amount2;
            sprintf(temp, "%s %d|%s %d", "fried-rice", ordered_amount1, "Egg-drop-soup", amount); 
        }
    }
    else{  //1. fried-rice 1 or Egg-drop-soup 1
        sscanf(ordered, "%s %d", ordered_meal1, &ordered_amount1);
        if (strcmp(ordered_meal1, meal) == 0){  //品項相同，僅增加amount
            amount += ordered_amount1;
            sprintf(temp, "%s %d", meal, amount);  
        }else{                                  //品項不同，新增品項跟amount進ordered string
            if (strcmp("fried-rice", meal) == 0){   // meal = "fried-rice", ordered_meal1 ="Egg-drop-soup"
                sprintf(temp, "%s %d|%s %d", "fried-rice", amount, "Egg-drop-soup", ordered_amount1);  
            }else{                              // meal = "Egg-drop-soup", ordered_meal1 ="fried-rice"
                sprintf(temp, "%s %d|%s %d", "fried-rice", ordered_amount1, "Egg-drop-soup", amount); 
            }
        }
    }
    return temp;
}


void sendback_message_to_client(int connfd, char* massage){
    char sendback_msg[BUFSIZE];
    memset(sendback_msg, 0, BUFSIZE);
    strcpy(sendback_msg, massage);   //he strcpy() function copies the string pointed by source (including the null character) to the destination.

    int n;
    if ((n = write(connfd, sendback_msg, BUFSIZE)) == -1){ //or strlen(sendback_msg)+1  (need to add '\0')
        errexit("Error: write()\n");
    }
}


//receiveMessage from client: shop list, order, confirm, cancel, Yes/No
void receive_mssage_from_client(int connfd, char* massage){
    memset(massage, 0, BUFSIZE);
    int rn;
    if ((rn = read(connfd, massage, BUFSIZE)) == -1)
        errexit("Error: read()\n");
}


void *get_order(void* connfd_temp){  
    char receiveMessage[BUFSIZE], msg[BUFSIZE];
    char order[6], meal[15];
    int amount = 0, total_money = 0;
    enum shop order_shop; // shop 0 = Dessert Shop, 1=Beverage Shop, 2=Diner
    int shop_distance[3] = {3, 5, 8};
    char ordered[BUFSIZE];  //紀錄已點物品
    memset(ordered, 0, BUFSIZE); 
    int shop_flag = 0;  //紀錄是否為第一次order
    int temp = 0, temp2 = 0;

    int connfd = (int) connfd_temp;

    while(1){  //cancel 或 confirm 才會跳出
        //receiveMessage from client: shop list, order, confirm, cancel 
        receive_mssage_from_client(connfd, receiveMessage);

        // 針對clinent輸入的選擇做處理
        if (strcmp(receiveMessage, "shop list") == 0){  //shop list
            //send back shop list to client
            sendback_message_to_client(connfd, "Dessert Shop:3km\n- cookie:60$|cake:80$\nBeverage Shop:5km\n- tea:40$|boba:70$\nDiner:8km\n- fried-rice:120$|Egg-drop-soup:50$");
        }else if(strcmp(receiveMessage, "confirm") == 0){  //confirm
            if (strcmp(ordered, "") == 0){
                sendback_message_to_client(connfd, "Please order some meals");
            }else{
                if (delivery_man_1_delivery_time <= delivery_man_2_delivery_time){  //將此costomer分派給所需時間較少的外送員
                    printf("delivery_man_1 go!!\n");
                    pthread_mutex_lock(&mutex1);
                    delivery_man_1_delivery_time += shop_distance[order_shop];  //protect????
                    pthread_mutex_unlock(&mutex1);

                    if (delivery_man_1_delivery_time > 30){
                        sendback_message_to_client(connfd,"Your delivery will take a long time, do you want to wait?");
                        //do{
                        receive_mssage_from_client(connfd, receiveMessage);
                       // }while((strcmp(receiveMessage, "No") != 0) && (strcmp(receiveMessage, "Yes") == 0));
                        if (strcmp(receiveMessage, "No") == 0){
                            pthread_mutex_lock(&mutex1);
                            delivery_man_1_delivery_time -= shop_distance[order_shop];  //protect????
                            pthread_mutex_unlock(&mutex1);
                            break;
                        }else{  //Yes
                            pthread_mutex_lock(&mutex3);
                            sleep(shop_distance[order_shop]);
                            delivery_man_1_customer += 1;
                            delivery_man_1_income += total_money; 

                            pthread_mutex_lock(&mutex1);
                            delivery_man_1_delivery_time -= shop_distance[order_shop];
                            pthread_mutex_unlock(&mutex1);
                            pthread_mutex_unlock(&mutex3);

                            sprintf(msg, "Delivery has arrived and you need to pay %d$", total_money); 
                            sendback_message_to_client(connfd, msg);
                            break;
                        }
                    }
                    // delivering, increase那一外送員的總costomer及income數，及sleep for delivering.
                    sendback_message_to_client(connfd, "Please wait a few minutes...");  //外送中，回傳等待中
                    /***  creitical section  **/
                    pthread_mutex_lock(&mutex3);
                    sleep(shop_distance[order_shop]);
                    delivery_man_1_customer += 1;
                    delivery_man_1_income += total_money; 
                    
                    pthread_mutex_lock(&mutex1);
                    delivery_man_1_delivery_time -= shop_distance[order_shop];
                    pthread_mutex_unlock(&mutex1);

                    pthread_mutex_unlock(&mutex3);
                    /**************************/
                    //local variables no need to protect
                    sprintf(msg, "Delivery has arrived and you need to pay %d$", total_money); 
                    sendback_message_to_client(connfd, msg);
                    break;
                }else{
                    printf("delivery_man_2 go!!\n");
                    pthread_mutex_lock(&mutex2);
                    delivery_man_2_delivery_time += shop_distance[order_shop];  
                    pthread_mutex_unlock(&mutex2);  

                    if (delivery_man_2_delivery_time > 30){
                        sendback_message_to_client(connfd,"Your delivery will take a long time, do you want to wait?"); 
                        //do{
                        receive_mssage_from_client(connfd, receiveMessage);
                        //}while((strcmp(receiveMessage, "No") != 0) && (strcmp(receiveMessage, "Yes") == 0));
                        if (strcmp(receiveMessage, "No") == 0){
                            pthread_mutex_lock(&mutex2);
                            delivery_man_2_delivery_time -= shop_distance[order_shop];  
                            pthread_mutex_unlock(&mutex2);
                            break;
                        }else{  //Yes
                            pthread_mutex_lock(&mutex4);
                            sleep(shop_distance[order_shop]);
                            delivery_man_2_customer += 1;
                            delivery_man_2_income += total_money; 

                            pthread_mutex_lock(&mutex2);
                            delivery_man_2_delivery_time -= shop_distance[order_shop];
                            pthread_mutex_unlock(&mutex2);

                            pthread_mutex_unlock(&mutex4);

                            sprintf(msg, "Delivery has arrived and you need to pay %d$", total_money);  //當外送到時，顯示付款金額
                            sendback_message_to_client(connfd, msg);
                            break;
                        }
                    }
                    // delivering, increase那一外送員的總costomer及income數，及sleep for delivering.
                    sendback_message_to_client(connfd, "Please wait a few minutes...");  //外送中，回傳等待中
                    /***  creitical section  **/
                    pthread_mutex_lock(&mutex4);
                    sleep(shop_distance[order_shop]);
                    delivery_man_2_customer += 1;
                    delivery_man_2_income += total_money; 

                    pthread_mutex_lock(&mutex2);
                    delivery_man_2_delivery_time -= shop_distance[order_shop];
                    pthread_mutex_unlock(&mutex2);

                    pthread_mutex_unlock(&mutex4);
                    /**************************/
                    //local variables no need to protect
                    sprintf(msg, "Delivery has arrived and you need to pay %d$", total_money);  //當外送到時，顯示付款金額
                    sendback_message_to_client(connfd, msg);
                    break;
                }
            }
        }else if(strcmp(receiveMessage, "cancel") == 0){  //cancel
            break;
        }else{  //order
            //ex: order tea 1
            sscanf(receiveMessage, "%s %s %d", order, meal, &amount);

            if (shop_flag){  //不是第一次order
                enum shop order_shop2 = meal_to_shop(meal);
                if (order_shop2 == order_shop){     //判斷新的order是否在同一家shop
                    char *new_ordered;
                    switch (order_shop2) //deal order's money
                    {
                        case Dessert:
                            total_money += Dessertshop_order(meal, amount);
                            new_ordered = produce_new_ordered_Dessert(ordered, meal, amount);
                            break;
                        case Beverage:
                            total_money += Beverageshop_order(meal, amount);
                            new_ordered = produce_new_ordered_Beverage(ordered, meal, amount);
                            break;
                        case Diner:
                            total_money += Dinershop_order(meal, amount);
                            new_ordered = produce_new_ordered_Diner(ordered, meal, amount);
                            break;
                        default:
                            break;
                    }
                    strcpy(ordered, new_ordered);  //還需判斷order的是不是同一物品, (amount需相加,品相不新增)
                    free(new_ordered);  //用完立馬free,避免interrupt
                }
                sendback_message_to_client(connfd, ordered);   // 不管新order有沒有在同一間店都print out已存的ordered
            }else{           //第一次order
                shop_flag = 1;
                order_shop = meal_to_shop(meal); //return Dessert or Beverage or Diner
                switch (order_shop)  //deal order's money
                {
                    case Dessert:
                        total_money += Dessertshop_order(meal, amount);
                        break;
                    case Beverage:
                        total_money += Beverageshop_order(meal, amount);
                        break;
                    case Diner:
                        total_money += Dinershop_order(meal, amount);
                        break;
                    default:
                        break;
                }
                sprintf(ordered, "%s %d", meal, amount);
                sendback_message_to_client(connfd, ordered); 
            }
        }
    } //跳出迴圈
    close(connfd);
}


//在 Server 端按下 Ctrl + C 後請產生"result.txt"後斷線並結束程式。
void CtrlCHandler(int signum) {
    // close(sockfd);
    // close(connfd);

    //destroy mutex
    printf("CtrlCHandler clean up mutex\n");
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    pthread_mutex_destroy(&mutex4);
    printf("Main thread completed\n");

    FILE *fptr;
    fptr = fopen("./result.txt","w");
    if(fptr == NULL)
    {
        printf("Error!");   
        exit(1);             
    }

    int total_costumer = 0, total_income = 0;
    total_costumer += delivery_man_1_customer;
    total_costumer += delivery_man_2_customer;;
    total_income += delivery_man_1_income;
    total_income += delivery_man_2_income;
    fprintf(fptr,"customer: %d\nincome: %d$",total_costumer, total_income);
    sleep(1);  //給他一點時間寫檔，不然會寫不到
    printf("customer: %d, income: %d$\n",total_costumer, total_income);


    fclose(fptr);
    exit(0);
}


int main(int argc , char *argv[])
{   
    if (argc != 2)
        errexit("Usage: %s <PORT>\n", argv[0]);   //ex: ./hw3 7000

    int sockfd, connfd; //socket descriptor
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    /*create socket and bind socket to port*/
    sockfd = passivesock(argv[1], "tcp", 50);  //maximum server request queue length: 50


    signal(SIGINT, CtrlCHandler);

    //initial mutex
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);
    pthread_mutex_init(&mutex4, NULL);


    int thread_nums = 0;
    int c = 0;
    pthread_t   thread[NUMTHREADS];
    while (1){
        /* waiting for connection, accept connection from clients */
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if (connfd == -1)
            errexit("Error: accept()\n");        
        //close(sockfd); /*close client connection*/ //避免其他人連?
        printf("# of Costomer had connected: %d\n",++c);


        int rc = pthread_create(&thread[thread_nums++], NULL, get_order, (void *)connfd); 
        checkResults("pthread_create()\n", rc);

        // wait thread complete 不會用到
        // if (thread_nums == NUMTHREADS){
        //     sleep(1); //Main thread wait a bit until all threads were done
        //     for(int i = 0; i < NUMTHREADS; i++){
        //         printf("pthread_join(thread[%d])\n", i);
        //         int rc = pthread_join(thread[i], NULL);
        //         checkResults("pthread_join()\n", rc);
        //     }
        //     break; //get out of while(1)
        // }
    }
    /*close client connection*/
    close(connfd);

    //destroy mutex
    printf("Main thread clean up mutex\n");
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex2);
    pthread_mutex_destroy(&mutex3);
    printf("Main thread completed\n");


    return 0;
}