#!/bin/sh
cp  u-boot/u-boot u-boot/u-boot.elf
rm -f boot.bin
bootgen -image boot.bif -o i boot.bin
