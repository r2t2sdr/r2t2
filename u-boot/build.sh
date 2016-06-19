#!/bin/sh
export CROSS_COMPILE=arm-xilinx-linux-gnueabi-
make mrproper
make zynq_zc70x_config
make

