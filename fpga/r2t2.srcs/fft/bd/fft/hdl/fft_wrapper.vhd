--Copyright 1986-2015 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2015.2 (lin64) Build 1266856 Fri Jun 26 16:35:25 MDT 2015
--Date        : Sat Nov 14 21:35:27 2015
--Host        : thinkpad running 64-bit Arch Linux
--Command     : generate_target fft_wrapper.bd
--Design      : fft_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity fft_wrapper is
  port (
    axis_rd_data_count : out STD_LOGIC_VECTOR ( 31 downto 0 )
  );
end fft_wrapper;

architecture STRUCTURE of fft_wrapper is
  component fft is
  port (
    axis_rd_data_count : out STD_LOGIC_VECTOR ( 31 downto 0 )
  );
  end component fft;
begin
fft_i: component fft
     port map (
      axis_rd_data_count(31 downto 0) => axis_rd_data_count(31 downto 0)
    );
end STRUCTURE;
