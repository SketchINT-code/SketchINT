GCC = g++
CFLAGS = -std=c++11 -O2 -w
FILES = heavyhitter heavychange latency_estimation inflated_latency_detection

all: $(FILES)

heavyhitter: heavyhitter.cpp
	$(GCC) $(CFLAGS) heavyhitter.cpp -o heavyhitter

heavychange: heavychange.cpp
	$(GCC) $(CFLAGS) heavychange.cpp -o heavychange

latency_estimation: latency_estimation.cpp
	$(GCC) $(CFLAGS) latency_estimation.cpp -o latency_estimation

inflated_latency_detection: inflated_latency_detection.cpp
	$(GCC) $(CFLAGS) inflated_latency_detection.cpp -o inflated_latency_detection

clean:
	-rm -f *.out $(FILES)
