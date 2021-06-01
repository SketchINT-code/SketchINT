# Per-flow Per-switch Tests

## Introduction

This directory contains source code of 4 per-flow per-switch tests -- per-switch heavy hitter detection, per-switch heavy change detection, latency estimation, and inflated latency detection. 

## How to run

Before running, modify `dataset directory` in the main function to your dataset directory in `./demo/`.
For the dataset file format, please refer to `pfps_test/sketch/host_sketches.h`.

After that, compile and run the program with the following commands:

```bash
$ cd demo
$ make
$ ./heavyhitter
$ ./heavychange
$ ./latency_estimation
$ ./inflated_latency_detection
```

