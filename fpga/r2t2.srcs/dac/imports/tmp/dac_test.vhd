library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library UNISIM;
use UNISIM.VComponents.all;

entity test_dac is
	Port (test : in std_logic_vector (0 downto 0));
end test_dac;

architecture Behavioral of test_dac is

	SIGNAL sysclk    : std_logic := '0';
	SIGNAL clk200Mhz : std_logic := '0';
	SIGNAL reset     : std_logic := '0';
	SIGNAL cnt       : unsigned(15 downto 0) := (others => '0'); 

	component radio_dac_if IS
		PORT( 
				DCLKIO            : IN   std_logic;
				S_AXIS_DAC_A_tdata  : IN   std_logic_vector (15 DOWNTO 0);
				S_AXIS_DAC_A_tvalid : IN   std_logic;
				S_AXIS_DAC_B_tdata  : IN   std_logic_vector (15 DOWNTO 0);
				S_AXIS_DAC_B_tvalid : IN   std_logic;
				clksys            : IN   std_logic;
				ebuf_rst          : IN   std_logic;
				clk_ioctrl_200MHz : IN   std_logic;
				DB                : OUT  std_logic_vector (13 DOWNTO 0)
			);

	END component radio_dac_if;


begin

	sysclk    <= not sysclk  after 4 ns;
	clk200Mhz <= not clk200Mhz after 2.5 ns;
	reset   <= '1' after 50 ns;

	dut : radio_dac_if 
	PORT MAP (
				 DCLKIO                => sysclk,
				 S_AXIS_DAC_A_tdata    => std_logic_vector(cnt), 
				 S_AXIS_DAC_A_tvalid   => '1',
				 S_AXIS_DAC_B_tdata    => std_logic_vector(cnt + 55), 
				 S_AXIS_DAC_B_tvalid   => '1',
				 clksys           	=> sysclk, 
				 ebuf_rst            => reset,
				 clk_ioctrl_200MHz   => clk200Mhz,
				 DB                  => OPEN 
			 );

	do : process (sysclk) 
	begin
		if sysclk = '1' then
			cnt <= cnt + 1;
		end if;
	end process;  


end Behavioral;
