----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 10/15/2015 04:54:04 PM
-- Design Name: 
-- Module Name: testb - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
library UNISIM;
use UNISIM.VComponents.all;


entity testb is
	Port ( test : in STD_LOGIC_VECTOR (0 downto 0));
end testb;


architecture Behavioral of testb is

	SIGNAL clk   : std_logic := '0';
	SIGNAL clk200Mhz : std_logic := '0';
	SIGNAL reset : std_logic := '0';

	SIGNAL dco_n : std_logic := '0';
	SIGNAL dco_p : std_logic;
	SIGNAL fco_n : std_logic := '0';
	SIGNAL fco_p : std_logic;
	
	SIGNAL adc_a_d0_n : std_logic := '0';
	SIGNAL adc_a_d0_p : std_logic;
	SIGNAL adc_a_d1_n : std_logic := '0';
	SIGNAL adc_a_d1_p : std_logic;
	SIGNAL adc_b_d0_n : std_logic := '0';
	SIGNAL adc_b_d0_p : std_logic;
	SIGNAL adc_b_d1_n : std_logic := '0';
	SIGNAL adc_b_d1_p : std_logic;

	SIGNAL cnt    : unsigned(7 downto 0) := (others => '0'); 
	
	component adc_lvds_if IS
		PORT( 
				LVDS_ADC_A_D0_N   : IN     std_logic;
				LVDS_ADC_A_D0_P   : IN     std_logic;
				LVDS_ADC_A_D1_N   : IN     std_logic;
				LVDS_ADC_A_D1_P   : IN     std_logic;
				LVDS_ADC_B_D0_N   : IN     std_logic;
				LVDS_ADC_B_D0_P   : IN     std_logic;
				LVDS_ADC_B_D1_N   : IN     std_logic;
				LVDS_ADC_B_D1_P   : IN     std_logic;
				LVDS_ADC_DCO_N    : IN     std_logic;
				LVDS_ADC_DCO_P    : IN     std_logic;
				LVDS_ADC_FCO_N    : IN     std_logic;
				LVDS_ADC_FCO_P    : IN     std_logic;
				sreset            : IN     std_logic;
				clk200Mhz         : IN     std_logic;
		        pl_clk            : IN     std_logic;
				M_AXIS_ACD_A_tdata  : OUT  std_logic_vector(15 DOWNTO 0);
				M_AXIS_ACD_B_tdata  : OUT  std_logic_vector(15 DOWNTO 0)
			);


	end component adc_lvds_if ;


begin

	clk     <= not clk  after 1 ns;
	clk200Mhz <= not clk200Mhz after 2.5 ns;
	reset   <= '1' after 50 ns;
	dco_n <= not dco_n after 1 ns;
	fco_n <= not fco_n after 4 ns;

	dco_p <= not(dco_n); 
	fco_p <= not(fco_n); 

	adc_a_d0_p <= not adc_a_d0_n;
	adc_a_d1_p <= not adc_a_d1_n;
	adc_b_d0_p <= not adc_b_d0_n;
	adc_b_d1_p <= not adc_b_d1_n;


	dut : adc_lvds_if 
	PORT MAP (
			sreset => reset,  
			clk200Mhz => clk200Mhz,
			pl_clk => '0',
			LVDS_ADC_A_D0_N => adc_a_d0_n,
			LVDS_ADC_A_D0_P => adc_a_d0_p,
			LVDS_ADC_A_D1_N => adc_a_d1_n,
			LVDS_ADC_A_D1_P => adc_a_d1_p,
			LVDS_ADC_B_D0_N => adc_b_d0_n,
			LVDS_ADC_B_D0_P => adc_b_d0_p,
			LVDS_ADC_B_D1_N => adc_b_d1_n,
			LVDS_ADC_B_D1_P => adc_b_d1_p,
			LVDS_ADC_DCO_N => dco_n,
			LVDS_ADC_DCO_P => dco_p,
			LVDS_ADC_FCO_N => fco_n,
			LVDS_ADC_FCO_P => fco_p 
			 );

	do : process (clk) 
	begin
		cnt <= cnt + 1;
		adc_a_d0_n <= cnt(0);
		adc_a_d1_n <= cnt(1);
		adc_b_d0_n <= cnt(2);
		adc_b_d1_n <= cnt(3);
	end process;  


end Behavioral;
