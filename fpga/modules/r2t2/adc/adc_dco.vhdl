LIBRARY ieee;
  USE ieee.std_logic_1164.all;
  USE ieee.NUMERIC_STD.all;
LIBRARY unisim;
  USE unisim.VCOMPONENTS.all;

ENTITY adc_dco_alignment IS
  PORT( 
    LVDS_ADC_DCO_N    : IN     std_logic;
    LVDS_ADC_DCO_P    : IN     std_logic;
    adc_clk           : OUT    std_logic;
    adc_clk_div       : OUT    std_logic
  );
END ENTITY adc_dco_alignment;

ARCHITECTURE struct OF adc_dco_alignment IS

  SIGNAL dco             : std_logic;

  COMPONENT BUFIO
  PORT (
    I : IN     std_ulogic;
    O : OUT    std_ulogic
  );
  END COMPONENT BUFIO;

  COMPONENT BUFR
  GENERIC (
    BUFR_DIVIDE : string := "BYPASS";
    SIM_DEVICE  : string := "VIRTEX4"
  );
  PORT (
    CE  : IN     std_ulogic;
    CLR : IN     std_ulogic;
    I   : IN     std_ulogic;
    O   : OUT    std_ulogic
  );
  END COMPONENT BUFR;

  COMPONENT IBUFDS_LVDS_25
  PORT (
    I  : IN     std_ulogic;
    IB : IN     std_ulogic;
    O  : OUT    std_ulogic
  );
  END COMPONENT IBUFDS_LVDS_25;

BEGIN

  DCO_BUFIO : BUFIO
    PORT MAP (
      O => adc_clk,
      I => dco
    );

  DCO_BUFR : BUFR
    GENERIC MAP (
      BUFR_DIVIDE => "4",
      SIM_DEVICE  => "7SERIES"
    )
    PORT MAP (
      O   => adc_clk_div,
      CE  => '1',
      CLR => '0',
      I   => dco
    );

  DCO_buf : IBUFDS_LVDS_25
    PORT MAP (
      O  => dco,
      I  => LVDS_ADC_DCO_P,
      IB => LVDS_ADC_DCO_N
    );


END ARCHITECTURE struct;
