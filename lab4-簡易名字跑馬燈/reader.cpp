#include <cstdio>      // fprintf(), perror()
#include <cstdlib>     // exit()
#include <cstring>     // memset()
#include <csignal>    // signal()
#include <fcntl.h>     // open()
#include <unistd.h>    // read(), write(), close()

#include <sys/socket.h> // socket(), connect()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h>  // htons()

int connfd, fd;

void sigint_handler(int signo) {
    close(fd);
    close(connfd);
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        fprintf(stderr, "Usage: ./reader <server_ip> <port> <device_path>");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, sigint_handler);

    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = inet_addr(argv[1]);
    cli_addr.sin_port = htons((u_short)atoi(argv[2]));

    if(connect(connfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr)) == -1) {
        perror("connect()");
        exit(EXIT_FAILURE);
    }

    if((fd = open(argv[3], O_RDWR)) < 0) {
        perror(argv[3]);
        exit(EXIT_FAILURE);
    }

    int ret;
    char buf[16] = {}; //int?

    while (true) {
        if((ret = read(fd, buf, sizeof(buf))) == -1) {
            perror("read()");
            exit(EXIT_FAILURE);
        }
        else{
            printf("reader read from driver(kernel) is buf = %s\n",buf);
        }

        for (int i = 0; i < 16; ++i) {
            printf("%d ", buf[i]);     //讀出來的 char buf[16] 是ASCII
        }
        printf("\n");

        if(write(connfd, buf, 16) == -1) {  //write to socket device, 是用socket device的write
            perror("write()");
            exit(EXIT_FAILURE);
        }

        sleep(1);
    }

    close(fd);
    close(connfd);
    return 0;
}
