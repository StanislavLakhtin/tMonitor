set target-async on
set confirm off
set history save
set mem inaccessible-by-default off
target extended-remote /dev/cu.usbmodemBED9AFE1
mon swdp_scan
att 1
load ./build/tMonitor.elf
file ./build/tMonitor.elf
start
