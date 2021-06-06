onbreak {quit -f}
onerror {quit -f}

vsim -t 1ps -lib xil_defaultlib ram_8_64k_opt

do {wave.do}

view wave
view structure
view signals

do {ram_8_64k.udo}

run -all

quit -force
