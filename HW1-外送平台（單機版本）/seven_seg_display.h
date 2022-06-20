#pragma once
#include <stdio.h> 
#include <sys/ioctl.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

void seven_seg_display(int fd, int total_money){  //display in decimal
    _7seg_info_t data;
    char r[4];
    int r_len = 0;
    

    r_len = sprintf((char* )r, "%d", total_money);  //int to string
    ioctl(fd ,_7SEG_IOCTL_ON , NULL);
    data.Mode = _7SEG_MODE_HEX_VALUE;
    switch(r_len) { 
        case 1: 
            data.Which = _7SEG_D8_INDEX;
            data.Value = r[0] - '0';  //char to int
            ioctl(fd , _7SEG_IOCTL_SET , &data);

            data.Which = _7SEG_D7_INDEX;
            data.Value = 0;  
            ioctl(fd , _7SEG_IOCTL_SET , &data);

            data.Which = _7SEG_D6_INDEX;
            data.Value = 0;  
            ioctl(fd , _7SEG_IOCTL_SET , &data);
            
            data.Which = _7SEG_D5_INDEX;
            data.Value = 0;  
            ioctl(fd , _7SEG_IOCTL_SET , &data);
            break;
        case 2: 
            data.Which = _7SEG_D8_INDEX;
            data.Value = r[1] - '0';  
            ioctl(fd , _7SEG_IOCTL_SET , &data);

            data.Which = _7SEG_D7_INDEX;
            data.Value = r[0] - '0';  
            ioctl(fd , _7SEG_IOCTL_SET , &data);

            data.Which = _7SEG_D6_INDEX;
            data.Value = 0;  
            ioctl(fd , _7SEG_IOCTL_SET , &data);
            
            data.Which = _7SEG_D5_INDEX;
            data.Value = 0;  
            ioctl(fd , _7SEG_IOCTL_SET , &data);
            break;
        case 3: 
            data.Which = _7SEG_D8_INDEX;
            data.Value = r[2] - '0';  //char to int
            ioctl(fd , _7SEG_IOCTL_SET , &data);
            
            data.Which = _7SEG_D7_INDEX;
            data.Value = r[1] - '0';  //char to int
            ioctl(fd , _7SEG_IOCTL_SET , &data);
            
            data.Which = _7SEG_D6_INDEX;
            data.Value = r[0] - '0';  //char to int
            ioctl(fd , _7SEG_IOCTL_SET , &data);

            data.Which = _7SEG_D5_INDEX;
            data.Value = 0;  
            ioctl(fd , _7SEG_IOCTL_SET , &data);  
            break;
        case 4: 
            data.Which = _7SEG_D5_INDEX;
            data.Value = r[0] - '0';  //char to int
            ioctl(fd , _7SEG_IOCTL_SET , &data);

            data.Which = _7SEG_D6_INDEX;
            data.Value = r[1] - '0';  //char to int
            ioctl(fd , _7SEG_IOCTL_SET , &data);

            data.Which = _7SEG_D7_INDEX;
            data.Value = r[2] - '0';  //char to int
            ioctl(fd , _7SEG_IOCTL_SET , &data);
            
            data.Which = _7SEG_D8_INDEX;
            data.Value = r[3] - '0';  //char to int
            ioctl(fd , _7SEG_IOCTL_SET , &data);
            break;
        default:
            printf("result out of display range\n");
            data.Which = _7SEG_ALL;
            data.Value = 0xffff;
            ioctl(fd , _7SEG_IOCTL_SET , &data);
            break;
    } 
}
