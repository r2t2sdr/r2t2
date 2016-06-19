LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.NUMERIC_STD.all;
LIBRARY unisim;
  USE unisim.VCOMPONENTS.all;

ENTITY adc_lvds_if IS
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
    resetn            : IN     std_logic;
    clk200Mhz         : IN     std_logic;
    pl_clk            : IN     std_logic;
    M_AXIS_ACD_A_tdata  : OUT  std_logic_vector(15 DOWNTO 0);
    M_AXIS_ACD_A_tvalid : OUT  std_logic;
    M_AXIS_ACD_B_tdata  : OUT  std_logic_vector(15 DOWNTO 0);
    M_AXIS_ACD_B_tvalid : OUT  std_logic
  );


END ENTITY adc_lvds_if ;

ARCHITECTURE struct OF adc_lvds_if IS

  SIGNAL adc_clk_div         : std_logic;
  SIGNAL bitslip             : std_logic;
  SIGNAL adc_clk             : std_logic;
  SIGNAL adc_A_tdata         : std_logic_vector(15 DOWNTO 0);
  SIGNAL adc_B_tdata         : std_logic_vector(15 DOWNTO 0);


  COMPONENT adc_data_channel
  PORT (
    LVDS_ADC_D0_N  : IN     std_logic;
    LVDS_ADC_D0_P  : IN     std_logic;
    LVDS_ADC_D1_N  : IN     std_logic;
    LVDS_ADC_D1_P  : IN     std_logic;
    adc_clk        : IN     std_logic;
    adc_clk_div    : IN     std_logic;
    bitslip        : IN     std_logic;
    reset          : IN     std_logic;
    adc_data       : OUT    std_logic_vector (15 DOWNTO 0)
  );
  END COMPONENT adc_data_channel;

  COMPONENT adc_dco_alignment
  PORT (
    LVDS_ADC_DCO_N    : IN     std_logic ;
    LVDS_ADC_DCO_P    : IN     std_logic ;
    adc_clk           : OUT    std_logic ;
    adc_clk_div       : OUT    std_logic 
  );
  END COMPONENT adc_dco_alignment;

  COMPONENT adc_fco_alignment
  PORT (
    LVDS_ADC_FCO_N  : IN     std_logic;
    LVDS_ADC_FCO_P  : IN     std_logic;
    adc_clk         : IN     std_logic;
    adc_clk_div     : IN     std_logic;
    reset           : IN     std_logic;
    clk200Mhz       : IN     std_logic;
	bitslip         : OUT    std_logic 
  );
  END COMPONENT adc_fco_alignment;



BEGIN

  M_AXIS_ACD_A_tdata <= adc_A_tdata;
  M_AXIS_ACD_B_tdata <= adc_B_tdata;  
  M_AXIS_ACD_A_tvalid <= '1';
  M_AXIS_ACD_B_tvalid <= '1';

  DCO_alignment : adc_dco_alignment
    PORT MAP (
      LVDS_ADC_DCO_N    => LVDS_ADC_DCO_N,
      LVDS_ADC_DCO_P    => LVDS_ADC_DCO_P,
      adc_clk           => adc_clk,
      adc_clk_div       => adc_clk_div
    );

  FCO_alignment : adc_fco_alignment
    PORT MAP (
      LVDS_ADC_FCO_N  => LVDS_ADC_FCO_N,
      LVDS_ADC_FCO_P  => LVDS_ADC_FCO_P,
      adc_clk         => adc_clk,
      adc_clk_div     => adc_clk_div,
	  reset           => resetn,
	  clk200Mhz       => clk200Mhz,
      bitslip         => bitslip
    );

    Data_ChannelA : adc_data_channel
      PORT MAP (
        LVDS_ADC_D0_N  => LVDS_ADC_A_D0_N,
        LVDS_ADC_D0_P  => LVDS_ADC_A_D0_P,
        LVDS_ADC_D1_N  => LVDS_ADC_A_D1_N,
        LVDS_ADC_D1_P  => LVDS_ADC_A_D1_P,
        adc_clk        => adc_clk,
        adc_clk_div    => adc_clk_div,
        bitslip        => bitslip,
		reset          => resetn, 
        adc_data       => adc_A_tdata
      );

    Data_ChannelB : adc_data_channel
      PORT MAP (
        LVDS_ADC_D0_N  => LVDS_ADC_B_D0_N,
        LVDS_ADC_D0_P  => LVDS_ADC_B_D0_P,
        LVDS_ADC_D1_N  => LVDS_ADC_B_D1_N,
        LVDS_ADC_D1_P  => LVDS_ADC_B_D1_P,
        adc_clk        => adc_clk,
        adc_clk_div    => adc_clk_div,
        bitslip        => bitslip,
		reset          => resetn, 
        adc_data       => adc_B_tdata
      );

END ARCHITECTURE struct;
