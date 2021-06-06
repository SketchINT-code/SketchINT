-makelib ies_lib/xil_defaultlib -sv \
  "E:/software/vivado2017.04/Vivado/2017.4/data/ip/xpm/xpm_memory/hdl/xpm_memory.sv" \
-endlib
-makelib ies_lib/xpm \
  "E:/software/vivado2017.04/Vivado/2017.4/data/ip/xpm/xpm_VCOMP.vhd" \
-endlib
-makelib ies_lib/blk_mem_gen_v8_4_1 \
  "../../../ipstatic/simulation/blk_mem_gen_v8_4.v" \
-endlib
-makelib ies_lib/xil_defaultlib \
  "../../../../ram_8_64k_ex.srcs/sources_1/ip/ram_8_64k/sim/ram_8_64k.v" \
-endlib
-makelib ies_lib/xil_defaultlib \
  glbl.v
-endlib

