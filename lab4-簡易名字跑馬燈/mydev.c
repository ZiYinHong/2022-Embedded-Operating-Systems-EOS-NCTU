// #include <string.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>


MODULE_LICENSE("GPL"); 

char readbuf[16];  
char writebuf[1];

char* convert_char_to_bitarray(char character){
    char* bitarray;
    switch (character)
    {   
        case 'A': bitarray="1111001100010001";return bitarray;
        case 'b': bitarray="1111001100010001";return bitarray;
        case 'C': bitarray="1100111100000000";return bitarray;
        case 'd': bitarray="0000011001000101";return bitarray;
        case 'E': bitarray="1000011100000001";return bitarray;
        case 'F': bitarray="1000001100000001";return bitarray;
        case 'G': bitarray="1001111100010000";return bitarray;
        case 'H': bitarray="0011001100010001";return bitarray;
        case 'I': bitarray="1100110001000100";return bitarray;
        case 'J': bitarray="1100010001000100";return bitarray;
        case 'K': bitarray="0000000001101100";return bitarray;
        case 'L': bitarray="0000111100000000";return bitarray;
        case 'M': bitarray="0011001110100000";return bitarray;
        case 'N': bitarray="0011001110001000";return bitarray;
        case 'O': bitarray="1111111100000000";return bitarray;
        case 'P': bitarray="1000001101000001";return bitarray;
        case 'q': bitarray="0111000001010000";return bitarray;
        case 'R': bitarray="1110001100011001";return bitarray;
        case 'S': bitarray="1101110100010001";return bitarray;
        case 'T': bitarray="1100000001000100";return bitarray;
        case 'U': bitarray="0011111100000000";return bitarray;
        case 'V': bitarray="0000001100100010";return bitarray;
        case 'W': bitarray="0011001100001010";return bitarray;
        case 'X': bitarray="0000000010101010";return bitarray;
        case 'Y': bitarray="0000000010100100";return bitarray;
        case 'Z': bitarray="1100110000100010";return bitarray;
        default: bitarray="0000000000000000";return bitarray;
    }
}

// File Operations
//返回：成功實際讀取的位元組數，失敗返回負值
//處理 :ex: "A"-> "1111001100010001", 再丟回去
static ssize_t my_read(struct file *fp , char *buf , size_t count , loff_t * fpos) {
    printk("call myread\n"); 

    //EX: writebuf[0]= A --> readbuf="1111001100010001"
    strncpy(readbuf, convert_char_to_bitarray(writebuf[0]), 16);
    
    // 從內核空間(readbuf)複製到使用者空間(buf)
    if(!copy_to_user(buf, readbuf, count)){ //返回：成功返回0，失敗返回沒有拷貝成功的資料位元組數
        printk("kernel send data: %s\n", readbuf);  //成功
        return count;
    }else{
        printk("kernel send data(my_read) failed\n");  //成功                                                  //失敗
        return -1;
    }
}

//返回：成功實際寫入的位元組數，失敗返回負值
static ssize_t my_write(struct file *fp ,const char *buf , size_t count , loff_t *fpos) {
    printk("call mywrite\n");

    // 從使用者空間(buf)複製到內核空間(writebuf)
    if(!copy_from_user(writebuf, buf, count)){   //成功
        printk("kernel receive data: %s\n", writebuf);
        return count;
    } else{
        printk("kernel receive data(my_write) failed\n");                                       //失敗
        return -1;   
    }                                        

}

struct file_operations my_fops = {
    read: my_read,
    write: my_write
    //open 用原本<linux/fs.h>的
};

#define MAJOR_NUM 244
#define DEVICE_NAME "mydev"

static int my_init(void) {
    printk("call my_init\n");
    if(register_chrdev(MAJOR_NUM , DEVICE_NAME , &my_fops) < 0) {
        printk("Can not get major %d\n", MAJOR_NUM);
        return(-EBUSY);
    }

    printk("My device is started and the major is %d\n", MAJOR_NUM);
    return 0;
}


static void my_exit(void) {
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk("call exit\n");
}

module_init(my_init);
module_exit(my_exit);