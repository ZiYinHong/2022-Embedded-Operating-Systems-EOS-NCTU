#include <iostream>
#include <string>

#include <stdio.h>  // perror()
#include <sys/fcntl.h>  // open()
#include <sys/ioctl.h>
#include <unistd.h> // close()
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

#include "seven_seg_display.h"
#include "led_display.h"
#include "calculation.h"

using namespace std;


void lcd_display(int fd, unsigned short key){
    lcd_write_info_t display;
    /* Save user input char and display the char*/
    display.Count = 1;
    display.Msg[0] = (char)(key & 0xff); 
    ioctl(fd ,LCD_IOCTL_WRITE, &display);
}

int main ()
{   
    unsigned short key; //keypad
    unsigned short data; //LED
    int fd;
    if((fd = open("/dev/lcd", O_RDWR)) < 0) {
        printf("fail to open /dev/lcd");
        return -1;
    }
    ioctl(fd ,LCD_IOCTL_CLEAR, NULL);       /* Clear LCD */ 
    ioctl(fd , KEY_IOCTL_CLEAR, &key);      /* Clear keypad */
    data = LED_ALL_OFF;
    ioctl(fd, LED_IOCTL_SET, &data);        /* Clear LED */
    
    printf("Start calculator program\n");
    for(int j = 0; j < 3; j++){  /*每次計算完成後，不須任何動作即可再進行下一次的輸入，自己設定最多輸入三次*/
        ioctl(fd , KEY_IOCTL_CLEAR, &key);      /* Clear keypad */

        string char_input, input;
        int index = 0;
        while(1){
            ioctl(fd, KEY_IOCTL_WAIT_CHAR, &key);
            cout << "user input is : " << (char)(key & 0xff) << "\n";
            
            
            if ((key & 0xff) == '#'){ 
                lcd_display(fd, (int)('='));
                break; /*and start calculation*/
            }else{
                switch((char)(key & 0xff)){
                    case 'A': 
                        lcd_display(fd, (int)('+'));
                        char_input[index++] = '+';
                        break;
                    case 'B': 
                        lcd_display(fd, (int)('-'));
                        char_input[index++] = '-';
                        break;
                    case 'C': 
                        lcd_display(fd, (int)('*'));
                        char_input[index++] = '*';
                        break;
                    case 'D': 
                        lcd_display(fd, (int)('/'));
                        char_input[index++] = '/';
                        break;
                    case '*':   // 使用者按下AC,清除整行，不算一次
                        char_input.clear();                /*清除已輸入的input*/
                        index = 0;
                        ioctl(fd ,LCD_IOCTL_CLEAR, NULL);  /* Clear LCD */ 
                        ioctl(fd , KEY_IOCTL_CLEAR, &key); /* Clear keypad */
                        break;
                    default: // 0~9
                        lcd_display(fd, key);
                        char_input[index++] = (char)(key & 0xff);
                        break;
                }
            }
        }
        char_input[index] = '\0';
        input = char_input.c_str(); // c-string to c++ string
        cout << "input fomula is :  " << input;  //print on console
        
        int result = calculation(input);
        cout << " = " << result << "\n"; //print on console
        
        /*7-seg display*/
        if (result >= 0)
            seven_seg_display(fd, result);

        /*led display*/
        if (result >= 0 && result <= 255)
            led_display(fd, result);
        else
            printf("result must be in 0~255 for LED display.\n"); 

        /*LCD display*/
        lcd_write_info_t display;
        display.Count = sprintf((char* )display.Msg, "%d", result);  //int to string
        ioctl(fd ,LCD_IOCTL_WRITE, &display);
        sleep(3); 
        
        /* 每次LCD Display完之後清除, cursor 回到 (0, 0) */ 
        ioctl(fd ,LCD_IOCTL_CLEAR, NULL);       /* Clear LCD */ 
        ioctl(fd , KEY_IOCTL_CLEAR, &key);      /* Clear keypad */
    } 
  
    ioctl(fd , _7SEG_IOCTL_OFF , NULL);        /* turn off all 7 seg*/
    data = LED_ALL_OFF;
    ioctl(fd, LED_IOCTL_SET, &data);        /* turn off all LED*/

    close(fd);
    return 0;
}
