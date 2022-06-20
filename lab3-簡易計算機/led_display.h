#pragma once
#include <bitset>
#include <stdio.h> 
#include <sys/ioctl.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

using namespace std;

void led_display(int fd, int result){
    unsigned short data9 = LED_D9_INDEX;
    unsigned short data10 = LED_D10_INDEX;
    unsigned short data11 = LED_D11_INDEX;
    unsigned short data12 = LED_D12_INDEX;
    unsigned short data13 = LED_D13_INDEX;
    unsigned short data14 = LED_D14_INDEX;
    unsigned short data15 = LED_D15_INDEX;
    unsigned short data16 = LED_D16_INDEX;

    int D9, D10, D11, D12, D13, D14, D15, D16, D17;

    //string binary = bitset<8>(result).to_string();
    bitset<8> binary(result);   //int to binary
    cout <<"result in binary :"<< binary << "\n";

    if (binary[0]){
        D16 = LED_IOCTL_BIT_SET;
    }else {
        D16 = LED_IOCTL_BIT_CLEAR;
    }
    
    if (binary[1]){
        D15 = LED_IOCTL_BIT_SET;
    }else {
        D15 = LED_IOCTL_BIT_CLEAR;
    }
    
    if (binary[2]){
        D14 = LED_IOCTL_BIT_SET;
    }else {
        D14 = LED_IOCTL_BIT_CLEAR;
    }
    
    if (binary[3]){
        D13 = LED_IOCTL_BIT_SET;
    }else {
        D13 = LED_IOCTL_BIT_CLEAR;
    }
    
    if (binary[4]){
        D12 = LED_IOCTL_BIT_SET;
    }else {
        D12 = LED_IOCTL_BIT_CLEAR;
    }
    
    if (binary[5]){
        D11 = LED_IOCTL_BIT_SET;
    }else {
        D11 = LED_IOCTL_BIT_CLEAR;
    }
    
    if (binary[6]){
        D10 = LED_IOCTL_BIT_SET;
    }else {
        D10 = LED_IOCTL_BIT_CLEAR;
    }

    if (binary[7]){
        D9 = LED_IOCTL_BIT_SET;
    }else {
        D9 = LED_IOCTL_BIT_CLEAR;
    }

    //display D9~D16
    ioctl(fd, D9, &data9);
    ioctl(fd, D10, &data10);
    ioctl(fd, D11, &data11);
    ioctl(fd, D12, &data12);
    ioctl(fd, D13, &data13);
    ioctl(fd, D14, &data14);
    ioctl(fd, D15, &data15);
    ioctl(fd, D16, &data16);
    // sleep(1);
}
