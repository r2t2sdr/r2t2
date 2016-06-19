------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2015 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.Radio_DAC_ebuffer.rtl
--
-- ABSTRACT:
--  Elastic buffer for Radio DAC clock domain crossing. The Data is 
--  provided by on the rising edge of the AD9716 DCLKIO.
--
-- HISTORY:
--  Date      Author      Description
--  20150119  mw          created
--
------------------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.NUMERIC_STD.all;
LIBRARY wdl_lib;
USE wdl_lib.wdl_def.all;

ENTITY Radio_DAC_ebuffer IS
	PORT( 
			S_AXIS_DAC_A_tdata  : IN  std_logic_vector (15 DOWNTO 0);
			S_AXIS_DAC_A_tvalid : IN  std_logic;
			S_AXIS_DAC_B_tdata  : IN  std_logic_vector (15 DOWNTO 0);
			S_AXIS_DAC_B_tvalid : IN  std_logic;
			clksys       : IN     std_logic;
			dac_clk      : IN     std_logic;
			ebuf_rst     : IN     std_logic;
			data_I       : OUT    std_logic_vector (13 DOWNTO 0);
			data_Q       : OUT    std_logic_vector (13 DOWNTO 0);
			data_ddr_rst : OUT    std_logic
		);

-- Declarations

END ENTITY Radio_DAC_ebuffer ;

--
ARCHITECTURE rtl OF Radio_DAC_ebuffer IS


	SIGNAL iData std_logic_vector(13 DOWNTO 0);
	SIGNAL qData std_logic_vector(13 DOWNTO 0);
BEGIN

	inData : PROCESS 
	BEGIN
	END inData
	-- concurrent ----------------------------------------------------------------------
	-- data_Q <= (OTHERS => '0');  -- not used, drive zero
	-- data_I <= rdata;
	data_Q <= rdata;
	data_I <= (OTHERS => '0'); 
	rd_buf_stat <= rd_buf_err & rd_upper_th & rd_lower_th;
	-- combinational process -----------------------------------------------------------
	-- clocked process -----------------------------------------------------------------
	--
	-- elastic buffer distributed RAM...
	ebuf_read_port : PROCESS(dac_clk)
	BEGIN
		IF dac_clk'EVENT AND dac_clk = '1' THEN
			rdata <= elastic_buf(TO_INTEGER(rd_ptr(rd_ptr'HIGH-1 DOWNTO 0)));
		END IF;
	END PROCESS;

	write_data : PROCESS(dac_clk)
	BEGIN
		IF clksys'EVENT AND clksys = '1' THEN
			IF we = '1' THEN
				elastic_buf(TO_INTEGER(waddr)) <= wdata;
			END IF;
		END IF;
	END PROCESS;
	--
	--
	read_data : PROCESS(clksys)
	BEGIN
		IF clksys'EVENT AND clksys = '1' THEN
			if S_AXIS_DAC_A_tvalid then
				iData <= S_AXIS_DAC_A_tdata;
			else
				iData <= (others => 0);
			end if;
			if S_AXIS_DAC_B_tvalid then
				qData <= S_AXIS_DAC_B_tdata;
			else
				qData <= (others => 0);
			end if;
		end if;
	END PROCESS;
END ARCHITECTURE rtl;
