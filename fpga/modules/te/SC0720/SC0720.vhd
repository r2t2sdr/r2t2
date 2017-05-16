----------------------------------------------------------------------------------
-- Company: Trenz Electronic GmbH
-- Engineer: Antti Lukats 
-- 
-- Create Date: 13.11.2013 09:58:03
-- Design Name: 
-- Module Name: SC0720 - Behavioral
-- Project Name: 
-- Target Devices: TE0720 SoM
-- Tool Versions: 2014.4
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.4  
-- Additional Comments:
-- 
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity SC0720 is Port ( 
    --
    -- External I2C
    --
	ext_sda_i      : in  std_logic;
    ext_sda_o      : out std_logic;
    ext_sda_t      : out std_logic;
    ext_scl_i      : in  std_logic;
    ext_scl_o      : out std_logic;
    ext_scl_t      : out std_logic;

    --
    --
    --
    PHY_LED0: out std_logic;
    PHY_LED1: out std_logic;
    PHY_LED2: out std_logic;
	--
	-- Connect to same name PL pin
	--	
	PL_pin_K16 :  in std_logic; -- PUDC
	PL_pin_K19 :  in std_logic; -- XCLK 
	PL_pin_L16 : out std_logic; -- X1 SCL out
	PL_pin_M15 :  in std_logic; -- X2
	PL_pin_N15 :  in std_logic; -- X3
	PL_pin_P16 :  in std_logic; -- X4
	PL_pin_P22 :  in std_logic; -- X5 SDA in
	PL_pin_K20 : out std_logic; -- X6
	PL_pin_N22 : out std_logic; -- X7 SDA out
    --
    -- Connect to EMIO I2C1
    --
	sda_i      : out  std_logic;
	sda_o      : in std_logic;
	sda_t      : in std_logic;
	scl_i      : out  std_logic;
	scl_o      : in std_logic;
	scl_t      : in std_logic
	);

	
	
end SC0720;

architecture Behavioral of SC0720 is

signal sda: std_logic;
signal scl: std_logic;

begin
	PL_pin_K20 <= '0'; -- TE0720-00 compat!
	
	-- I2C bus merger
	ext_sda_o <= sda_o;
	ext_sda_t <= sda_t;
	ext_scl_t <= scl_t;	
	
	
	-- SDA readback from SC to I2C core
	sda_i 	<= PL_pin_P22 and ext_sda_i;
	-- SDA/SCL pass through to SC	
	PL_pin_N22 <= sda;
	PL_pin_L16 <= scl;
	-- internal signals
	sda 	<= sda_o or sda_t;
	scl 	<= scl_o or scl_t;
	-- SCL feedback to I2C core
	scl_i 	<= scl;
	--
	--
	--
    PHY_LED0 <= PL_pin_M15;
    PHY_LED1 <= PL_pin_N15;
    PHY_LED2 <= PL_pin_P16;

	
end Behavioral;
