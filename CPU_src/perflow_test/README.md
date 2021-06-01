# Per-flow Accuracy Tests

## Introduction

For traditional per-flow tasks, we test the performance of our Tower, Tower+CU against CM, CU, and Elastic, on six tasks: flow size estimation, heavy hitter detection, heavy change detection, flow size distribution, entropy estimation, and cardinality estimation. Specifically:

- The `./tower/` directory contains all the tests of Tower, Tower+CU, CM and CU. We implemented our Tower's data structure and query operations in `tower.h`, and since CM and CU share similar data structures and operations with Tower, we combine the implementations of CM and CU into `tower.h`.
- The `./elastic/` directory contains all the tests of Elastic.
- The `./common/` directory contains implementations of `BOBHash32` and MRAC algorithm (used for flow size distribution).



## Dataset

We use `CAIDA18` dataset for our experiments. The dataset is publicly available at https://www.caida.org/data/overview/. We cut two 5s traces (the second trace is immediately after the first trace) as input data.

Before running the demo, please download the dataset and put them into the `../../data/` directory.



## How to make and run the test

- Compilations: To compile test codes of Tower, Tower+CU, or CM, CU, just run: 

  ```shell
  cd tower
  make
  ```

  Similarly, to compile test codes of Elastic, just run:

  ```shell
  cd elastic
  make
  ```

- Configurations: before running the tests, you have to specify all the configurations in `./tower/config.txt` and `./elastic/config.txt`. We have provided you with a sample configuration file in `./tower` and `./elastic`: it reads data from `../../data/trace1.dat` (and `../../data/trace2.dat` for heavy change detection), and outputs the results to `./result.txt`. If you would like to specify your custom configurations, please refer to `./configuration.md`.

- Running the test: after configurations, just run

  ```
  cd tower
  ./tower_test.out
  ```

  and
  
  ```
  cd elastic
  ./elastic_test.out
  ```
  
  

