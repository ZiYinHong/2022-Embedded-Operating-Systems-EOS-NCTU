CFLAGS = gcc
SRC = sockop.c sockop.h

all: hw2 hw2_client

hw2: hw2.c $(SRC)
	$(CFLAGS) -o hw2 hw2.c sockop.c 

hw2_client: hw2_client.c $(SRC)
	$(CFLAGS) -o hw2_client hw2_client.c sockop.c