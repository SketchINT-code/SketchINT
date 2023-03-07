CC = g++
CFLAGS = -O2 -std=c++11 -w

OBJS = tower_test.out

all: $(OBJS)

tower_test.out: tower_test.cpp
	$(CC) $(CFLAGS) -o tower_test.out tower_test.cpp

clean:
	rm -f $(OBJS)