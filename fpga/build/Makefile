.PHONY: bitstream

bitstream:
	vivado -mode batch -source bitstream.tcl

clean:
	rm -f vivado*
	rm -f webtalk*
	rm -f *~
	vivado -mode batch -source reset.tcl

