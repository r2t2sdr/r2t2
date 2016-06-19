#!/bin/sh

# build kernel
#files von dts/linux nach dts kompieren

export CROSS_COMPILE=arm-xilinx-linux-gnueabi-
make ARCH=arm r2t2linux_defconfig # ohne FreeRTOS
make ARCH=arm LOADADDR=0x8000 uImage -j4   # ohne FreeRTOS
make ARCH=arm modules -j4

cp arch/arm/boot/dts/linux/* arch/arm/boot/dts/
make ARCH=arm system.dtb 
make ARCH=arm INSTALL_MOD_PATH=/tftpboot modules_install

mv ./arch/arm/boot/dts/system.dtb devicetree.dtb

# create boot image

arm-xilinx-eabi-objcopy -O binary -R .comment -S vmlinux vmlinux.bin
mkimage -f r2t2.its image.ub
cp image.ub /tftpboot/
