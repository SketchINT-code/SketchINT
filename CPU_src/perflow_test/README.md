# Per-flow Accuracy Tests

## Introduction

For traditional per-flow tasks, we test the performance of our Tower, Tower+CU against CM, CU, CM(O), CU(O), Elastic, UnivMon, NitroSketch on six tasks: flow size estimation, heavy hitter detection, heavy change detection, flow size distribution, entropy estimation, and cardinality estimation. And we test the performance of NitroSketch and UnivMon on the first four tasks. Specifically:

- The `./tower/` directory contains all the tests of Tower, Tower+CU, CM and CU, CM(O), and CU(O). We implemented our Tower's data structure and query operations in `tower.h`, and since CM/CM(O) and CU/CU(O) share similar data structures and operations with Tower, we integrate their implementations into `tower.h`.
- The `./elastic/` directory contains all the tests of Elastic.
- The `./NitroSketch/` directory contains all the tests of NitroSketch.
- The `./UnivMon/` directory contains all the tests of UnivMon.
- The `./common/` directory contains implementations of `BOBHash32` and MRAC algorithm (used for flow size distribution).



## Dataset

We use `CAIDA18` dataset for our experiments. The dataset is publicly available at https://www.caida.org/data/overview/. We cut two 5s traces (the second trace is immediately after the first trace) as input data.

Before running the demo, please download the dataset and put them into the `../../data/` directory.



## How to make and run the test

- Compilations: To compile test codes of Tower, Tower+CU, or CM/CM(O), CU/CU(O), just run: 

  ```shell
  cd tower
  make
  ```

  Similarly, to compile test codes of Elastic, NitroSketch and UnivMon, just run:

  ```shell
  cd elastic
  make
  ```
  
  ```shell
  cd NitroSketch
  make
  ```
  
  ```shell
  cd UnivMon
  make
  ```

- Configurations: before running the tests, you have to specify all the configurations in `./tower/config.txt` , `./elastic/config.txt` , `./NitroSketch/config.txt` and `./UnivMon/config.txt`. We have provided you with a sample configuration file in `./tower` , `./elastic` , `./NitroSketch` and `./UnivMon`: it reads data from `../../data/trace1.dat` (and `../../data/trace2.dat` for heavy change detection), and outputs the results to `./result.txt`. If you would like to specify your custom configurations, please refer to `./configuration.md`.

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
  
  ```
  cd NitroSketch
  ./tower_test.out
  ```
  
  ```
  cd UnivMon
  ./tower_test.out
  ```

