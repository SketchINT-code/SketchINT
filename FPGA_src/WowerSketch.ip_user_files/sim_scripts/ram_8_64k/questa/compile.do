vlib questa_lib/work
vlib questa_lib/msim

vlib questa_lib/msim/blk_mem_gen_v8_4_1
vlib questa_lib/msim/xil_defaultlib

vmap blk_mem_gen_v8_4_1 questa_lib/msim/blk_mem_gen_v8_4_1
vmap xil_defaultlib questa_lib/msim/xil_defaultlib

vlog -work blk_mem_gen_v8_4_1 -64 \
"../../../ipstatic/simulation/blk_mem_gen_v8_4.v" \

vlog -work xil_defaultlib -64 \
"../../../../WowerSketch.srcs/sources_1/ip/ram_8_64k/sim/ram_8_64k.v" \


vlog -work xil_defaultlib \
"glbl.v"

