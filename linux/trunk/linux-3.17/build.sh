#!/bin/sh

# build kernel

export CROSS_COMPILE=arm-xilinx-linux-gnueabi-
make ARCH=arm r2t2_defconfig  # mit FreeRTOS
make ARCH=arm LOADADDR=0x02008000 uImage -j4 # mit FreeRTOS
make ARCH=arm modules -j4
cp arch/arm/boot/dts/freertos/* arch/arm/boot/dts/
make ARCH=arm system-top.dtb 
make ARCH=arm INSTALL_MOD_PATH=/tftpboot modules_install

mv ./arch/arm/boot/dts/system-top.dtb devicetree.dtb

# create boot image

arm-xilinx-eabi-objcopy -O binary -R .comment -S vmlinux vmlinux.bin
mkimage -f r2t2.its image.ub
cp image.ub /tftpboot/
