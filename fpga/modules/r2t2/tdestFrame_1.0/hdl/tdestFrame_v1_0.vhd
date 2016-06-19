library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tdestFrame_v1_0 is
	generic (
				C_AXIS_TDATA_WIDTH	: integer	:= 32;
				RX_ID	: integer	:= 0 
			);
	port (
			 axis_aclk	: in std_logic;
			 axis_aresetn	: in std_logic;

			 s00_axis_tready	: out std_logic;
			 s00_axis_tdata	: in std_logic_vector(C_AXIS_TDATA_WIDTH-1 downto 0);
			 s00_axis_tlast	: in std_logic;
			 s00_axis_tvalid	: in std_logic;

			 m00_axis_tvalid	: out std_logic;
			 m00_axis_tdata	: out std_logic_vector(C_AXIS_TDATA_WIDTH-1 downto 0);
			 m00_axis_tlast	: out std_logic;
			 m00_axis_tready	: in std_logic
		 );
end tdestFrame_v1_0;

architecture arch_imp of tdestFrame_v1_0 is

	signal sendHeader : std_logic := '0';

begin

	process(clk)  
	begin

		if s00_axis_tlast = '0' then
			sendHeader <= '1';
		else 
		end if;

		if sendHeader = '1'  then
			s00_axis_tready	<= '0'; 
			m00_axis_tvalid <= '1';
			m00_axis_tdata	 <= std_logic_vector(C_HEADER_VAL);
			m00_axis_tlast	 <= '0'; 
			sendHeader <= '0';
		else
			s00_axis_tready	<= m00_axis_tready; 
			m00_axis_tvalid <= s00_axis_tvalid;
			m00_axis_tdata	 <= s00_axis_tdata; 
			m00_axis_tlast	 <= s00_axis_tlast; 
		end if;

	end process;


end arch_imp;
