CC = g++
CFLAGS = -O2 -std=c++11 -w -pthread

OBJS = tower_test

all: $(OBJS)

tower_test: tower_test.cpp
	$(CC) $(CFLAGS) -o tower_test tower_test.cpp

clean:
	rm -f $(OBJS)
