CFLAGS = gcc
SRC = sockop.c sockop.h

all: lab6_server_shm lab6_server_thread lab6_client lab6_server

lab6_server: lab6_server.c $(SRC)
	$(CFLAGS) -o lab6_server lab6_server.c sockop.c

lab6_server_thread: lab6_server_thread.c $(SRC)
	$(CFLAGS) -o lab6_server_thread lab6_server_thread.c sockop.c -lpthread

lab6_server_shm: lab6_server_shm.c $(SRC)
	$(CFLAGS) -o lab6_server_shm lab6_server_shm.c sockop.c 

lab6_client: lab6_client.c $(SRC)
	$(CFLAGS) -o lab6_client lab6_client.c sockop.c