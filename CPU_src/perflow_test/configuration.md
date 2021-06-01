# Custom Configuration

In this repository, we have provided you with sample configuration files: `./tower/config.txt` and `./elastic/config.txt`. If you would like to set some custom configurations, here is an explanation of the configuration format:

For `./tower`:

```
DATA_TRACE_PATH   OUTPUT_FILE    # your trace file path; the name of file that you store your results
T   # number of test cases. In each test cases, you specify the size of memory for the sketch, and the application task you would like to do
MEMORY_USAGE(KB)   APPLICATION_ID   # allocated memory for the sketch; which application task to do (flow size estimation: 0, heavy hitter: 1, heavy change: 2, flow size distribution: 3, entropy: 4, and cardinality estimation: 5)
...
```

For `./elastic`, please first specify the memory usage in `./elastic/utils.h`, and the rest of configuration is as follows:

```
DATA_TRACE_PATH   OUTPUT_FILE
T
APPLICATION_ID
...
```



