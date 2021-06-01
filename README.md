# SketchINT: Empowering INT with TowerSketch for Per-flow Per-switch Measurement


## Introduction

Network measurement is indispensable to network operations. Two most promising measurement solutions are In-band Network Telemetry (INT) solutions and sketching solutions. INT solutions provide fine-grained per-switch per-packet information at the cost of high network overhead. Sketching solutions have low network overhead but fail to achieve both simplicity and accuracy for per-flow measurement. To keep their advantages, and at the same time, overcome their shortcomings, we first design SketchINT to combine INT and sketches, aiming to obtain all per-flow per-switch information with low network overhead. Second, for deployment flexibility and measurement accuracy, we design a new sketch for SketchINT, namely TowerSketch, which achieves both simplicity and accuracy. The key idea of TowerSketch is to use different-sized counters for different arrays under the property that the number of bits used for different arrays stays the same. TowerSketch can automatically record larger flows in larger counters and smaller flows in smaller counters. We have fully implemented our SketchINT prototype on a testbed consisting of 10 switches. We also implement our TowerSketch on P4, single-core CPU, multi-core CPU, and FPGA platforms to verify its deployment flexibility. Extensive experimental results verify that 1) TowerSketch achieves better accuracy than prior art on various tasks, outperforming the state-of-the-art ElasticSketch (SIGCOMM’18) up to 13.9 times in terms of error; 2) Compared to INT, SketchINT reduces the number of packets in the collection process by 3∼4 orders of magnitude with an error smaller than 5%.


## Repository Structure

*  `CPU_src/`: the source code of TowerSketch on CPU platform, containing local per-flow tasks, global per-flow per-switch tasks, and throughput tests on both single-core and multi-core CPU.
*  `FPGA_src/`: the source code of TowerSketch on FPGA platform.
*  `P4_src/`: the source code of TowerSketch on P4 platform, and the source code of SketchINT on P4 switch.
*  `Datasets/`: the example datasets, containing CAIDA for local tasks, and the dataset we dump from our testbed.


## Requirements

- make
- g++


## Reference

[1] The CAIDA Anonymized Internet Traces. http://www.caida.org/data/overview/.

[2] ElasticSketch. https://github.com/BlockLiu/ElasticSketchCode/.

