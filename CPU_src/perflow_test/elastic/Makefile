CC = g++
CFLAGS = -O2 -std=c++11 -w

OBJS = elastic_test.out

all: $(OBJS)

elastic_test.out: elastic_test.cpp
	$(CC) $(CFLAGS) -o elastic_test.out elastic_test.cpp

clean:
	rm -f $(OBJS)