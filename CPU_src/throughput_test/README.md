# Throughput Tests

## Introduction

This directory contains source code of throughput tests on single-core CPU and multi-core CPU. Specifically:

- The `./single-core/` directory contains throughput tests of Tower, Tower+CU, CM and CU on single-core CPU.
- The `./multi-core-lock/` directory contains implementations of the lock version TowerSketch.
- The `./multi-core-lock-free/` directory contains implementations of the lock-free version TowerSketch.

## How to run

First, `cd` to the corresponding directory.

Before running, modify `path to dataset` in the main function (in `accuracy.cpp`/`throughput.cpp`) to your dataset file.

After that, compile and run the program with the following commands:

```bash
$ make
$ ./accuracy
$ ./throughput
```

