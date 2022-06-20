#include <cstdio>      // fprintf(), perror()
#include <cstdlib>     // exit()
#include <cstring>     // c_str()
#include <csignal>    // signal()
#include <fcntl.h>     // open()
#include <unistd.h>    // read(), write(), close()
#include <string>      // c++ string
#include <string.h>    // c strncpy

using namespace std;

int main(int argc, char *argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Usage: ./writer <name> <device_path>");
        exit(EXIT_FAILURE);
    }
    
    int fd;
    if((fd = open(argv[2], O_RDWR)) < 0) {
        perror(argv[2]);
        exit(EXIT_FAILURE);
    }

    while (1){ //重頭(第一個字母)開始
        char buf[1] = {};
        string name = argv[1];
        int i = 0;

        //每隔一秒，就將新的字母寫到 driver 當中。
        while (i < name.length()) {
            sprintf((char *)buf, "%c", name[i++]);  

            if(write(fd, buf, sizeof(buf)) == -1) {
                perror("write()");  
                exit(EXIT_FAILURE);
            }
            sleep(1);
        }
    }


    close(fd);
    return 0;
}