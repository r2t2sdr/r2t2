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

SIGNAL tdata   : std_logic_vector(31 downto 0);

component rxtest_wrapper is
 
  
   port (
            
        M_AXIS_DATA_tdata : out STD_LOGIC_VECTOR ( 31 downto 0 );
        M_AXIS_DATA_tdest : out STD_LOGIC_VECTOR ( 3 downto 0 );
        M_AXIS_DATA_tlast : out STD_LOGIC_VECTOR ( 0 to 0 );
        M_AXIS_DATA_tready : in STD_LOGIC_VECTOR ( 0 to 0 );
        M_AXIS_DATA_tvalid : out STD_LOGIC_VECTOR ( 0 to 0 );
        aclk : in STD_LOGIC;
        aresetn : in STD_LOGIC_VECTOR ( 0 to 0 )    
  );
end component rxtest_wrapper;

begin

clk   <= not clk  after 4 ns;
reset <= '1' after 50 ns;


dut : rxtest_wrapper 
PORT MAP (
   aclk => clk,
   M_AXIS_DATA_tvalid(0) => valid,
   M_AXIS_DATA_tdata => tdata,
   M_AXIS_DATA_tready(0) => '1',
   aresetn(0) => reset
   );
   
process (clk) 
begin

end process;  


end Behavioral;
