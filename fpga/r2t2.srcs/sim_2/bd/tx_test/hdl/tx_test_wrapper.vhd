--Copyright 1986-2015 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2015.2 (lin64) Build 1266856 Fri Jun 26 16:35:25 MDT 2015
--Date        : Sun Oct 25 11:26:25 2015
--Host        : thinkpad running 64-bit Arch Linux
--Command     : generate_target tx_test_wrapper.bd
--Design      : tx_test_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity tx_test_wrapper is
  port (
    M00_AXIS_TX_tdata : out STD_LOGIC_VECTOR ( 15 downto 0 );
    M00_AXIS_TX_tready : in STD_LOGIC_VECTOR ( 0 to 0 );
    M00_AXIS_TX_tvalid : out STD_LOGIC_VECTOR ( 0 to 0 );
    M01_AXIS_TX_tdata : out STD_LOGIC_VECTOR ( 15 downto 0 );
    M01_AXIS_TX_tready : in STD_LOGIC_VECTOR ( 0 to 0 );
    M01_AXIS_TX_tvalid : out STD_LOGIC_VECTOR ( 0 to 0 );
    aclk : in STD_LOGIC;
    aresetn : in STD_LOGIC
  );
end tx_test_wrapper;

architecture STRUCTURE of tx_test_wrapper is
  component tx_test is
  port (
    aclk : in STD_LOGIC;
    aresetn : in STD_LOGIC;
    M00_AXIS_TX_tdata : out STD_LOGIC_VECTOR ( 15 downto 0 );
    M00_AXIS_TX_tready : in STD_LOGIC_VECTOR ( 0 to 0 );
    M00_AXIS_TX_tvalid : out STD_LOGIC_VECTOR ( 0 to 0 );
    M01_AXIS_TX_tdata : out STD_LOGIC_VECTOR ( 15 downto 0 );
    M01_AXIS_TX_tready : in STD_LOGIC_VECTOR ( 0 to 0 );
    M01_AXIS_TX_tvalid : out STD_LOGIC_VECTOR ( 0 to 0 )
  );
  end component tx_test;
begin
tx_test_i: component tx_test
     port map (
      M00_AXIS_TX_tdata(15 downto 0) => M00_AXIS_TX_tdata(15 downto 0),
      M00_AXIS_TX_tready(0) => M00_AXIS_TX_tready(0),
      M00_AXIS_TX_tvalid(0) => M00_AXIS_TX_tvalid(0),
      M01_AXIS_TX_tdata(15 downto 0) => M01_AXIS_TX_tdata(15 downto 0),
      M01_AXIS_TX_tready(0) => M01_AXIS_TX_tready(0),
      M01_AXIS_TX_tvalid(0) => M01_AXIS_TX_tvalid(0),
      aclk => aclk,
      aresetn => aresetn
    );
end STRUCTURE;
