#pragma once
#include <stdio.h> 
#include <sys/ioctl.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

using namespace std;

void led_display(int fd, int distance){
    unsigned short data9 = LED_D9_INDEX;
    unsigned short data10 = LED_D10_INDEX;
    unsigned short data11 = LED_D11_INDEX;
    unsigned short data12 = LED_D12_INDEX;
    unsigned short data13 = LED_D13_INDEX;
    unsigned short data14 = LED_D14_INDEX;
    unsigned short data15 = LED_D15_INDEX;
    unsigned short data16 = LED_D16_INDEX;
    unsigned short dataall = LED_ALL_ON;

    switch (distance)
    {
        case 3:
            ioctl(fd, LED_IOCTL_BIT_SET, &data9);
            ioctl(fd, LED_IOCTL_BIT_SET, &data10);
            ioctl(fd, LED_IOCTL_BIT_SET, &data11);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data11);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data10);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data9);
            break;
        case 5:
            ioctl(fd, LED_IOCTL_BIT_SET, &data9);
            ioctl(fd, LED_IOCTL_BIT_SET, &data10);
            ioctl(fd, LED_IOCTL_BIT_SET, &data11);
            ioctl(fd, LED_IOCTL_BIT_SET, &data12);
            ioctl(fd, LED_IOCTL_BIT_SET, &data13);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data13);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data12);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data11);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data10);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data9);
            break;
        case 8:
            ioctl(fd, LED_IOCTL_SET, &dataall);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data16);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data15);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data14);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data13);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data12);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data11);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data10);
            sleep(1);
            ioctl(fd, LED_IOCTL_BIT_CLEAR, &data9);
            break;
        default:
            break;
    }
}
