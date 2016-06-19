#!/bin/sh

rm arch/arm/boot/dts/r2t2.dtb 

export CROSS_COMPILE=arm-xilinx-linux-gnueabi-
make ARCH=arm r2t2_defconfig
make ARCH=arm LOADADDR=0x8000 uImage -j4
make ARCH=arm modules -j4
make ARCH=arm r2t2.dtb


make ARCH=arm INSTALL_MOD_PATH=/tftpboot modules_install

cp arch/arm/boot/dts/r2t2.dtb devicetree.dtb

# create boot image
arm-xilinx-eabi-objcopy -O binary -R .comment -S vmlinux vmlinux.bin
mkimage -f boot.its image.ub
cp image.ub /tftpboot/
