#ifndef _SOCKOP_H_
#define _SOCKOP_H_

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define errexit(format,arg...) exit(printf(format,##arg))

/* create server */
int passivesock(const char *service, const char *transport, int qlen);

/* connect to server */
int connectsock(const char *host, const char *service, const char *transport);

#endif
