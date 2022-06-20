// server 
#include <stdio.h>   //perror()
#include <stdlib.h>  //exit()
#include <string.h>
#include <unistd.h>  //dup2(), exec..()

#include <signal.h>
#include <sys/wait.h>

#include "sockop.h"

# define BUFSIZE 256

enum shop{Dessert, Beverage, Diner}; 

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
        if (strcmp("cookie", meal) == 0){  //品項1相同，增加品項1amount，品項2不變
            amount += ordered_amount1;
            sprintf(temp, "%s %d|%s %d", "cookie", amount, "cake", ordered_amount2);  
        }else{                                  //品項2相同，增加品項2amount，品項1不變
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
    char* temp = malloc(BUFSIZE);  //cause we need to return string(char*)
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
    printf("server send to client:\n%s\n", sendback_msg);  //for debug

    int n;
    if ((n = write(connfd, sendback_msg, BUFSIZE)) == -1){ //or strlen(sendback_msg)+1  (need to add '\0')
        errexit("Error: write()\n");
    }
}

void CtrlCHandler(int signum) {
     exit(0);
}

int main(int argc , char *argv[])
{   
    int sockfd, connfd; //socket descriptor
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    char receiveMessage[BUFSIZE], msg[BUFSIZE];
    char order[6], meal[15];
    int amount, total_money = 0;
    enum shop order_shop; // shop 0 = Dessert Shop, 1=Beverage Shop, 2=Diner
    int shop_distance[3] = {3, 5, 8};
    char ordered[BUFSIZE];  //紀錄已點物品
    int shop_flag = 0;
    int rn, wn;


    if (argc != 2)
        errexit("Usage: %s <PORT>\n", argv[0]);

    /*create socket and bind socket to port*/
    sockfd = passivesock(argv[1], "tcp", 10);

    while (1){
        /* waiting for connection, accept connection from clients */
        connfd = accept(sockfd, (struct sockaddr *)&addr_cln, &sLen);
        if (connfd == -1)
            errexit("Error: accept()\n");        
        //close(sockfd); /*close client connection*/ //避免其他人連?
        

        memset(ordered, 0, BUFSIZE);  //每一次新的client連上，variable都清零
        shop_flag = 0;
        total_money = 0;
        while(1){
            signal(SIGINT, CtrlCHandler); //signal user press ctrl-C
            //receiveMessage from client: shop list, order, confirm, cancel 
            memset(receiveMessage, 0, BUFSIZE);
            if ((rn = read(connfd, receiveMessage, BUFSIZE)) == -1)
                errexit("Error: read()\n");


            // 針對clinent輸入的選擇做處理
            if (strcmp(receiveMessage, "shop list") == 0){
                //send back shop list to client
                sendback_message_to_client(connfd, "Dessert Shop:3km\n- cookie:60$|cake:80$\nBeverage Shop:5km\n- tea:40$|boba:70$\nDiner:8km\n- fried-rice:120$|Egg-drop-soup:50$");
            }else if(strcmp(receiveMessage, "confirm") == 0){
                if (strcmp(ordered, "") == 0){
                    sendback_message_to_client(connfd, "Please order some meals");
                }else{
                    sendback_message_to_client(connfd, "Please wait a few minutes...");  //外送中，回傳等待中
                    sleep(shop_distance[order_shop]);
                    sprintf(msg, "Delivery has arrived and you need to pay %d$", total_money); 
                    sendback_message_to_client(connfd, msg);//當外送到時，顯示付款金額
                    break;
                }
            }else if(strcmp(receiveMessage, "cancel") == 0){
                break;
            }else{  //order
                //ex: order tea 1
                sscanf(receiveMessage, "%s %s %d", order, meal, &amount);

                //判斷是否在同一家shop
                if (shop_flag){  //不是第一次order
                    enum shop order_shop2 = meal_to_shop(meal);
                    if (order_shop2 == order_shop){  //new order shop = old order shop
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
        }
        close(connfd);
    }

    return 0;
}