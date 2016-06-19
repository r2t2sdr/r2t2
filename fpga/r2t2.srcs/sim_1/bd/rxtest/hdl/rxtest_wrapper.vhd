--Copyright 1986-2015 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2015.2 (lin64) Build 1266856 Fri Jun 26 16:35:25 MDT 2015
--Date        : Sat Nov  7 19:23:04 2015
--Host        : thinkpad running 64-bit Arch Linux
--Command     : generate_target rxtest_wrapper.bd
--Design      : rxtest_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity rxtest_wrapper is
  port (
    dout : out STD_LOGIC_VECTOR ( 31 downto 0 )
  );
end rxtest_wrapper;

architecture STRUCTURE of rxtest_wrapper is
  component rxtest is
  port (
    dout : out STD_LOGIC_VECTOR ( 31 downto 0 )
  );
  end component rxtest;
begin
rxtest_i: component rxtest
     port map (
      dout(31 downto 0) => dout(31 downto 0)
    );
end STRUCTURE;
