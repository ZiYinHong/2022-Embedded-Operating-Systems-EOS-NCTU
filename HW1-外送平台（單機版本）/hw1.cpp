#include <iostream> //cout
#include <string>

#include <string.h>  //strcpy()
#include <stdio.h>  // perror()
#include <sys/fcntl.h>  // open()
#include <sys/ioctl.h>
#include <unistd.h> // close()
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

#include "seven_seg_display.h"
#include "led_display.h"

using namespace std;

// LCD display number 0~9(user input number)
void lcd_display_num(int fd, unsigned short key){
    lcd_write_info_t display;
    /* Save user input char and display the char*/
    display.Count = 1;
    display.Msg[0] = (char)(key & 0xff); 
    ioctl(fd ,LCD_IOCTL_WRITE, &display);
}

// LCD display string ex: display order list
void lcd_display_str(int fd, string str){
    ioctl(fd ,LCD_IOCTL_CLEAR, NULL);       /* Clear LCD */

    lcd_write_info_t display;
    strcpy((char*)display.Msg, str.c_str());
    display.Count = str.length();  //int to string
    ioctl(fd ,LCD_IOCTL_WRITE, &display);
}


// return user choice number 1. or 2. or ..., or input order number
string store_user_input(int fd){
    string char_input, input;
    unsigned short key; //keypad
    int i = 0;
    ioctl(fd , KEY_IOCTL_CLEAR, &key);      /* Clear keypad */

    ioctl(fd, KEY_IOCTL_WAIT_CHAR, &key); // wait for user input
    cout << "user input is : " << (char)(key & 0xff) << "\n";
    while((key & 0xff) != '#'){
        lcd_display_num(fd, key);
        char_input[i++] = (char)(key & 0xff);
        ioctl(fd, KEY_IOCTL_WAIT_CHAR, &key);
        cout << "user input is : " << (char)(key & 0xff) << "\n";
    }
    char_input[i] = '\0';
    input = char_input.c_str(); // c-string to c++ string
    return input;
}

// string number -> int number
int read_num(string char_input){
    int i = 0;
    int num = 0;
    while(isdigit(char_input[i])){
        num = num*10+(char_input[i] - '0');
        i++;
    }
    return num;
}

// deal with order, return order money
int Dessertshop_order_food(int fd, string choose){
    // user order product(choose = 1 or 2)
    int money = 0;

    lcd_display_str(fd, "How many?\n");

    switch (choose[0])
    {
        case '1':   //ex: cookie  
            money = 60*read_num(store_user_input(fd));  // return 要下單的數量
            break;
        case '2':   //ex: cake
            money = 80*read_num(store_user_input(fd));  // return 要下單的數量
            break;
        default:
            break;
    }
    lcd_display_str(fd,"Please choose 1~4\n1.cookie 60$\n2.cake 80$\n3.confirm\n4.cancel\n");
    cout << "money = " << money <<"\n";
    return money;
}

int Beverageshop_order_food(int fd, string choose){
    // user order product(choose = 1 or 2)
    int money = 0;

    lcd_display_str(fd, "How many?\n");

    switch (choose[0])
    {
        case '1':   //tea    
            money = 40*read_num(store_user_input(fd));  // return 要下單的數量
            break;
        case '2':   //boba
            money = 70*read_num(store_user_input(fd));  // return 要下單的數量
            break;
        default:
            break;
    }
    lcd_display_str(fd,"Please choose 1~4\n1.tea 40$\n2.boba 70$\n3.confirm\n4.cancel\n");
    cout << "money = " << money <<"\n";
    return money;
}

int Dinershop_order_food(int fd, string choose){
    // user order product(choose = 1 or 2)
    int money = 0;

    lcd_display_str(fd, "How many?\n");

    switch (choose[0])
    {
        case '1':   //fried rice         
            money = 120*read_num(store_user_input(fd));  // return 要下單的數量
            break;
        case '2':   //Egg-drop soup
            money = 50*read_num(store_user_input(fd));  // return 要下單的數量
            break;
        default:
            break;
    }
    lcd_display_str(fd,"Please choose 1~4\n1.fried rice 120$\n2.Egg-drop soup 50$\n3.confirm\n4.cancel\n");
    return money;
}

int main ()
{   
    unsigned short key;
    string choose; //choose 會重複被用
    unsigned short data; //LED
    int fd;
    if((fd = open("/dev/lcd", O_RDWR)) < 0) {
        printf("fail to open /dev/lcd");
        return -1;
    }

    while(1){  
        printf("Start foodpanda program!\n");
        ioctl(fd ,LCD_IOCTL_CLEAR, NULL);       /* Clear LCD */ 
        ioctl(fd , KEY_IOCTL_CLEAR, &key);      /* Clear keypad */
        //ioctl(fd , _7SEG_IOCTL_OFF , NULL);   /* turn off all 7 seg*/
        seven_seg_display(fd, 0);             /* 7 seg -> 0000*/
        data = LED_ALL_OFF;
        ioctl(fd, LED_IOCTL_SET, &data);        /* Clear LED */
        //for one person order
        /*LCD display 主選單
            1. shop list
            2. order
        */
        lcd_display_str(fd, "1. shop list\n2. order\n");

        /*wait for user choose 1. or 2.*/
        choose = store_user_input(fd);  //return user input number in string
        cout << "choose[0] = " << choose[0] <<"\n";

        if (choose[0] == '1'){
            //  display shop list
            /* 
                dessert shop: 3km
                Beverage Shop: 5km
                Diner: 8km
            */
            lcd_display_str(fd, "dessert shop: 3km\nBeverage Shop: 5km\nDiner: 8km\n");
        
            //按任意鍵回主選單
            ioctl(fd, KEY_IOCTL_WAIT_CHAR, &key); 
            cout << "user input is : " << (char)(key & 0xff) << "\n";
        }else if (choose[0] == '2'){
            // display order list 餐廳選單
            /* 
                Please choose 1~3
                1. Dessert shop
                2. Beverage Shop
                3. Diner
            */
            lcd_display_str(fd, "Please choose 1~3\n1. Dessert shop\n2. Beverage Shop\n3. Diner\n");
            // then goto 訂餐選單

            /*wait for user choose 1. 2. 3. shop*/
            choose = store_user_input(fd);
            int total_money = 0; 
            switch(choose[0]){
                case '1': //Dessert shop
                    lcd_display_num(fd, key);
                    lcd_display_str(fd,"Please choose 1~4\n1.cookie 60$\n2.cake 80$\n3.confirm\n4.cancel\n");
                    /* choose product
                        Please choose 1~4
                            1.cookie 60$
                            2.cake 80$
                            3.confirm
                            4.cancel
                    */   
                    choose = store_user_input(fd); //return when user enter '#', return previous enter number in string
                    while((choose[0] != '3') && (choose[0] != '4')){
                        total_money += Dessertshop_order_food(fd, choose);
                        cout << "total_money = " << total_money << "\n";
                        choose = store_user_input(fd);
                        cout << "choose = " << choose << "\n";
                    }
                    if (choose[0] == '3'){
                        printf("total money = %d\n", total_money);
                        if (total_money == 0){ // not order any food, 回到主選單
                            break;
                        }else{
                            //confirm: start delivering,led顯示進度;7段顯示器顯示金額
                            seven_seg_display(fd, total_money);
                            lcd_display_str(fd,"Please wait for few minutes...\n");
                            int distance = 3;
                            led_display(fd, distance);
                            lcd_display_str(fd,"please pick up your meal\n");  //當餐點送達(led全滅)後
                            
                            //按任意鍵回主選單
                            ioctl(fd, KEY_IOCTL_WAIT_CHAR, &key);
                            cout << "user input is : " << (char)(key & 0xff) << "\n";
                            break;
                        }
                    }
                    if (choose[0] == '4'){
                        //cancel: 回到主選單
                        break;
                    }
                case '2': //Beverage Shop
                    lcd_display_num(fd, key);
                    lcd_display_str(fd,"Please choose 1~4\n1.tea 40$\n2.boba 70$\n3.confirm\n4.cancel\n");
                    /* choose product
                        Please choose 1~4
                            1.tea 40$
                            2.boba 70$
                            3.confirm
                            4.cancel
                    */
                    choose = store_user_input(fd); //return when user enter '#', return previous enter number in string
                    while((choose[0] != '3') && (choose[0] != '4')){
                        total_money += Beverageshop_order_food(fd, choose);
                        cout << "total_money = " << total_money << "\n";
                        choose = store_user_input(fd);
                    }
                    if (choose[0] == '3'){
                        printf("total money = %d\n", total_money);
                        if (total_money == 0){ // not order any food, 回到主選單
                            break;
                        }else{
                            //confirm: start delivering,led顯示進度;7段顯示器顯示金額
                            seven_seg_display(fd, total_money);
                            lcd_display_str(fd,"Please wait for few minutes...\n");
                            int distance = 5;
                            led_display(fd, distance);
                            lcd_display_str(fd,"please pick up your meal\n");  //當餐點送達(led全滅)後
                            
                            //按任意鍵回主選單
                            ioctl(fd, KEY_IOCTL_WAIT_CHAR, &key);
                            cout << "user input is : " << (char)(key & 0xff) << "\n";
                            break;
                        }
                    }
                    if (choose[0] == '4'){
                        //cancel: 回到主選單
                        break;
                    }
                case '3': //Diner
                    lcd_display_num(fd, key);
                    lcd_display_str(fd,"Please choose 1~4\n1.fried rice 120$\n2.Egg-drop soup 50$\n3.confirm\n4.cancel\n");
                    /* choose product
                        Please choose 1~4
                            1.fried rice 120$
                            2.Egg-drop soup 50$
                            3.confirm
                            4.cancel
                    */
                    choose = store_user_input(fd); //return when user enter '#', return previous enter number in string
                    while((choose[0] != '3') && (choose[0] != '4')){
                        total_money += Dinershop_order_food(fd, choose);
                        cout << "total_money = " << total_money << "\n"; 
                        choose = store_user_input(fd);
                    }
                    if (choose[0] == '3'){
                        printf("total money = %d\n", total_money);
                        if (total_money == 0){ // not order any food, 回到主選單
                            break;
                        }else{
                            //confirm: start delivering,led顯示進度;7段顯示器顯示金額
                            seven_seg_display(fd, total_money);
                            lcd_display_str(fd,"Please wait for few minutes...\n");
                            int distance = 8;
                            led_display(fd, distance);
                            lcd_display_str(fd,"please pick up your meal\n");  //當餐點送達(led全滅)後

                            //按任意鍵回主選單
                            ioctl(fd, KEY_IOCTL_WAIT_CHAR, &key);
                            cout << "user input is : " << (char)(key & 0xff) << "\n";
                            break;
                        }
                    }
                    if (choose[0] == '4'){
                        //cancel: 回到主選單
                        break;
                    }                        
                default: 
                    break;  // other ?
            }
        }else{
            // display 回主選單
        }
    }

    return 0;
}

