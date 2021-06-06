onbreak {quit -force}
onerror {quit -force}

asim -t 1ps +access +r +m+ram_8_64k -L blk_mem_gen_v8_4_1 -L xil_defaultlib -L unisims_ver -L unimacro_ver -L secureip -O5 xil_defaultlib.ram_8_64k xil_defaultlib.glbl

do {wave.do}

view wave
view structure

do {ram_8_64k.udo}

run -all

endsim

quit -force
