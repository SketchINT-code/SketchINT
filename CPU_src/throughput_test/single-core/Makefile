GCC = g++
CFLAGS = -std=c++11 -O2
FILES = accuracy throughput

all: $(FILES) 

accuracy: accuracy.cpp
	$(GCC) $(CFLAGS) accuracy.cpp -o accuracy

throughput: throughput.cpp
	$(GCC) $(CFLAGS) throughput.cpp -o throughput

clean:
	rm $(all) -f *~ *.o *.out
