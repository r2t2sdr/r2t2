LIBRARY ieee;
  USE ieee.std_logic_1164.all;
  USE ieee.NUMERIC_STD.all;
LIBRARY unisim;
  USE unisim.VCOMPONENTS.all;

ENTITY adc_fco_align_ctrl IS
  PORT( 
    adc_clk_div     : IN     std_logic;
    fco_serdes      : IN     std_logic_vector (7 DOWNTO 0);
    bitslip         : OUT    std_logic;
    serdes_ce       : OUT    std_logic;
    serdes_rst      : OUT    std_logic
  );
END ENTITY adc_fco_align_ctrl ;

--
ARCHITECTURE rtl OF adc_fco_align_ctrl IS
  --CONSTANT fco_pattern_c  : std_logic_vector(7 DOWNTO 0) := "11110000";
  CONSTANT fco_pattern_c  : std_logic_vector(7 DOWNTO 0) := "00001111";
  SIGNAL pattern_match    : std_logic := '0';
  SIGNAL alignment_done   : std_logic := '0';
  SIGNAL timer              : unsigned(1 DOWNTO 0) := "00";
  SIGNAL fco_align_ce       : std_logic := '0';

BEGIN

  PROCESS(adc_clk_div)
  BEGIN
    IF adc_clk_div'EVENT AND adc_clk_div = '1' THEN
      -- SERDES control...
      serdes_rst    <= '0';
      serdes_ce     <= '1';
      -- timer counting 16-bits...
      fco_align_ce <= '0';
	  timer <= timer + 1;
	  IF timer = 3 THEN
		  fco_align_ce <= '1';
	  END IF;

      -- Pattern comparator...
      IF fco_serdes = fco_pattern_c THEN
        pattern_match <= '1';
      ELSE
        pattern_match <= '0';
      END IF;

      bitslip <= '0';   -- default 
      IF fco_align_ce = '1' THEN
        IF pattern_match = '0' THEN
          bitslip <= '1';
        ELSE
          alignment_done <= '1';
        END IF;
      END IF;
    END IF;
  END PROCESS;
END ARCHITECTURE rtl;
