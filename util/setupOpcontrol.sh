sudo opcontrol --setup  -e L3_CACHE_MISSES:500:0xF7:0:1 -e DRAM_ACCESSES:500:0x3F:0:1 -e READ_REQUEST_L3_CACHE:500:0x07:0:1 --no-vmlinux  -e L3_EVICTIONS:500:0x0F:0:1 --session-dir=/tmp/joe/oprofile
