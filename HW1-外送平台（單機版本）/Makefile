CFLAGS = arm-unknown-linux-gnu-g++
SRC = seven_seg_display.h led_display.h
INCLUDE = -L /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/ \
-I /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/ \
-I /home/michelle/microtime/linux/include

hw1: hw1.cpp $(SRC)
	$(CFLAGS) -o hw1 hw1.cpp $(INCLUDE)

