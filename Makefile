.PHONY: 	all clean fsbl kernel fpga

all: 	fpga kernel uboot fsbl boot 

dts:
	hsi -source build_dts.tcl

bsp:
	hsi -source build_bsp.tcl

boot: 
	rm -f output/boot.bin
	bootgen -image boot.bif -o i output/boot.bin

fsbl:
	cd fsbl; hsi -source build.tcl
	cp -av fsbl/patch/* fsbl/src/
	touch fsbl/src/*.c
	cd fsbl/src; make -j1 

fsbl_clean:
	rm -rf fsbl/src

uboot:
	cd u-boot; make zynq_zc70x_config
	cd u-boot; make
	cp  u-boot/u-boot u-boot/u-boot.elf
	rm -f boot.bin

uboot_clean:
	cd u-boot; make mrproper

kernel:
	cd linux/trunk/linux-4.0-adi/; make -j1 -f Makefile.r2t2

kernel_clean:
	cd linux/trunk/linux-4.0-adi/; make clean

fpga:
	cd fpga/build; make -j1 bitstream
	cp fpga/build/r2t2* output/

fpga_clean:
	cd fpga/build; make -j1 clean

clean: 
	rm -f *~
	rm -f hsi*
	rm -f ps7_*
	rm -f ps_*
	rm -f svn-commit*

cleanall: clean kernel_clean fpga_clean uboot_clean fsbl_clean
	rm -r output/*
