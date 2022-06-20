.phony: clean
CFLAGS = gcc
SRC = sockop.c sockop.h

all: hw3_client hw3_thread hw3_tester


hw3_client: hw3_client.c $(SRC)
	$(CFLAGS) -o hw3_client hw3_client.c sockop.c

hw3_tester: hw3_tester.c $(SRC)
	$(CFLAGS) -o hw3_tester hw3_tester.c -lpthread

hw3_thread: hw3_thread.c $(SRC)
	$(CFLAGS) -o hw3_thread hw3_thread.c sockop.c -lpthread

clean:
	rm -f hw3_client hw3_thread hw3_tester