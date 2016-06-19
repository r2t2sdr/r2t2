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
SIGNAL valid : std_logic;
SIGNAL dest  : std_logic;
SIGNAL reset : std_logic := '0';

SIGNAL tdata   : std_logic_vector(63 downto 0);

component tx_test_wrapper is
 
  
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
end component tx_test_wrapper;

begin

clk   <= not clk  after 4 ns;
reset <= '1' after 50 ns;


dut : tx_test_wrapper 
PORT MAP (
   aclk => clk,
   aresetn => reset,   
   M00_AXIS_TX_tready(0) => '1',
   M01_AXIS_TX_tready(0) => '1'
   
  
   );
   
process (clk) 
begin

end process;  


end Behavioral;
