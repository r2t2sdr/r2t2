set prompt  (zynq-gdb)
set print pretty

target remote localhost:1235
b main
b Xil_Assert
b main.c:93
