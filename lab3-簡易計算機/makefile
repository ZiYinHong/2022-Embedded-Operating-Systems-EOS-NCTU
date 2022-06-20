CFLAGS = arm-unknown-linux-gnu-g++
SRC = seven_seg_display.h led_display.h calculation.h
INCLUDE = -L /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/ \
-I /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/ \
-I /home/michelle/microtime/linux/include

lab3: lab3.cpp $(SRC)
	$(CFLAGS) -o lab3 lab3.cpp $(INCLUDE)

