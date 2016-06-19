------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2015 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.adc_data_channel_ctrl.rtl
--
-- ABSTRACT:
--  Data channel interface for the LVDS ADC.
--  The ADC is expected to work in 16-bit DDR, Two-Lane, 1 x Frame, bytewise Mode 
--  (default for the AD9253).
--
-- HISTORY:
--  Date      Author      Description
--  20140909  mw          created
--
------------------------------------------------------------------------------------

LIBRARY ieee;
  USE ieee.std_logic_1164.all;
  USE ieee.NUMERIC_STD.all;
LIBRARY wdl_lib;
  USE wdl_lib.wdl_def.all;

ENTITY adc_data_channel_ctrl IS
  PORT( 
    adc_clk_div    : IN     std_logic;
    d0_serdes      : IN     std_logic_vector (7 DOWNTO 0);
    d1_serdes      : IN     std_logic_vector (7 DOWNTO 0);
    dco_is_aligned : IN     std_logic;
    fco_is_aligned : IN     std_logic;
    adc_data       : OUT    std_logic_vector (15 DOWNTO 0);
    data_valid     : OUT    std_logic;
    serdes_ce      : OUT    std_logic;
    serdes_rst     : OUT    std_logic
  );

-- Declarations

END ENTITY adc_data_channel_ctrl ;

--
ARCHITECTURE rtl OF adc_data_channel_ctrl IS
  SIGNAL stream_enb   : std_logic := '0';
  SIGNAL channel_data : std_logic_vector(15 DOWNTO 0) := (OTHERS => '0');
BEGIN
-- concurrent ----------------------------------------------------------------------
  data_valid <= stream_enb;
  adc_data   <= channel_data;
-- combinational process -----------------------------------------------------------
-- clocked process -----------------------------------------------------------------
  PROCESS(adc_clk_div)
  BEGIN
    IF adc_clk_div'EVENT AND adc_clk_div = '1' THEN
      -- SERDES control...
      serdes_rst    <= NOT dco_is_aligned;
      serdes_ce     <= dco_is_aligned;
      -- output register
      stream_enb <= dco_is_aligned AND fco_is_aligned;
      channel_data <= d1_serdes & d0_serdes;
    END IF;
  END PROCESS;
END ARCHITECTURE rtl;
