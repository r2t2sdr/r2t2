LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.NUMERIC_STD.all;
LIBRARY unisim;
USE unisim.VCOMPONENTS.all;
LIBRARY secureip;


ENTITY adc_fco_alignment IS
	PORT( 
			LVDS_ADC_FCO_N  : IN     std_logic;
			LVDS_ADC_FCO_P  : IN     std_logic;
			adc_clk         : IN     std_logic;
			adc_clk_div     : IN     std_logic;
			reset           : IN     std_logic;
	        clk200MHz 		: IN     std_logic;	
			bitslip         : OUT    std_logic
);

END ENTITY adc_fco_alignment ;

ARCHITECTURE struct OF adc_fco_alignment IS

	SIGNAL CNTVALUEIN : std_logic_vector(4 DOWNTO 0);
	SIGNAL adc_clk_n  : std_logic;
	SIGNAL fco_dly    : std_logic;
	SIGNAL fco_p      : std_logic;
	SIGNAL fco_par    : std_logic_vector(7 DOWNTO 0);
	SIGNAL fco_serdes : std_logic_vector(7 DOWNTO 0);
	SIGNAL serdes_ce  : std_logic;
	SIGNAL serdes_rst : std_logic;
	SIGNAL bitslip_internal : std_logic;
	SIGNAL iDelayRdy  : std_logic;

	COMPONENT ISERDESE2
		GENERIC (
		DATA_RATE         : string;
		DATA_WIDTH        : integer;
		DYN_CLKDIV_INV_EN : string;
		DYN_CLK_INV_EN    : string;
		INIT_Q1           : bit;
		INIT_Q2           : bit;
		INIT_Q3           : bit;
		INIT_Q4           : bit;
		INTERFACE_TYPE    : string;
		IOBDELAY          : string;
		NUM_CE            : integer;
		OFB_USED          : string;
		SERDES_MODE       : string;
		SRVAL_Q1          : bit;
		SRVAL_Q2          : bit;
		SRVAL_Q3          : bit;
		SRVAL_Q4          : bit
	);
	PORT (
			 BITSLIP      : IN     std_ulogic;
			 CE1          : IN     std_ulogic;
			 CE2          : IN     std_ulogic;
			 CLK          : IN     std_ulogic;
			 CLKB         : IN     std_ulogic;
			 CLKDIV       : IN     std_ulogic;
			 CLKDIVP      : IN     std_ulogic;
			 D            : IN     std_ulogic;
			 DDLY         : IN     std_ulogic;
			 DYNCLKDIVSEL : IN     std_ulogic;
			 DYNCLKSEL    : IN     std_ulogic;
			 OCLK         : IN     std_ulogic;
			 OCLKB        : IN     std_ulogic;
			 OFB          : IN     std_ulogic;
			 RST          : IN     std_ulogic;
			 SHIFTIN1     : IN     std_ulogic;
			 SHIFTIN2     : IN     std_ulogic;
			 O            : OUT    std_ulogic;
			 Q1           : OUT    std_ulogic;
			 Q2           : OUT    std_ulogic;
			 Q3           : OUT    std_ulogic;
			 Q4           : OUT    std_ulogic;
			 Q5           : OUT    std_ulogic;
			 Q6           : OUT    std_ulogic;
			 Q7           : OUT    std_ulogic;
			 Q8           : OUT    std_ulogic;
			 SHIFTOUT1    : OUT    std_ulogic;
			 SHIFTOUT2    : OUT    std_ulogic
		 );
	END COMPONENT ISERDESE2;
	COMPONENT IBUFDS_LVDS_25
		PORT (
				 I  : IN     std_ulogic;
				 IB : IN     std_ulogic;
				 O  : OUT    std_ulogic
			 );
	END COMPONENT IBUFDS_LVDS_25;

	COMPONENT IDELAYCTRL
		PORT (
				 REFCLK : IN     std_ulogic;
				 RST    : IN     std_ulogic;
				 RDY    : OUT    std_ulogic
			 );
	END COMPONENT IDELAYCTRL;

	COMPONENT IDELAYE2
		GENERIC (
					CINVCTRL_SEL          : string  := "FALSE";
					DELAY_SRC             : string  := "IDATAIN";
					HIGH_PERFORMANCE_MODE : string  := "FALSE";
					IDELAY_TYPE           : string  := "FIXED";
					IDELAY_VALUE          : integer := 0;
					PIPE_SEL              : string  := "FALSE";
					REFCLK_FREQUENCY      : real    := 200.0;
					SIGNAL_PATTERN        : string  := "DATA"
				);
		PORT (
				 C           : IN     std_ulogic;
				 CE          : IN     std_ulogic;
				 CINVCTRL    : IN     std_ulogic;
				 CNTVALUEIN  : IN     std_logic_vector (4 DOWNTO 0);
				 DATAIN      : IN     std_ulogic;
				 IDATAIN     : IN     std_ulogic;
				 INC         : IN     std_ulogic;
				 LD          : IN     std_ulogic;
				 LDPIPEEN    : IN     std_ulogic;
				 REGRST      : IN     std_ulogic;
				 CNTVALUEOUT : OUT    std_logic_vector (4 DOWNTO 0);
				 DATAOUT     : OUT    std_ulogic
			 );
	END COMPONENT IDELAYE2;

	COMPONENT adc_fco_align_ctrl
		PORT (
				 adc_clk_div     : IN     std_logic ;
				 fco_serdes      : IN     std_logic_vector (7 DOWNTO 0);
				 bitslip         : OUT    std_logic ;
				 serdes_ce       : OUT    std_logic ;
				 serdes_rst      : OUT    std_logic 
			 );
	END COMPONENT adc_fco_align_ctrl;


BEGIN

	fco_serdes <= fco_par;
	CNTVALUEIN <= (OTHERS => '0');
	adc_clk_n <= NOT(adc_clk);

  -- Instance port mappings.
	FCO_p_SERDES : ISERDESE2
	GENERIC MAP (
					DATA_RATE         => "DDR",
					DATA_WIDTH        => 8,
					DYN_CLKDIV_INV_EN => "FALSE",
					DYN_CLK_INV_EN    => "FALSE",
					INIT_Q1           => '0',
					INIT_Q2           => '0',
					INIT_Q3           => '0',
					INIT_Q4           => '0',
					INTERFACE_TYPE    => "NETWORKING",
					IOBDELAY          => "BOTH",
					NUM_CE            => 2,
					OFB_USED          => "FALSE",
					SERDES_MODE       => "MASTER",
					SRVAL_Q1          => '0',
					SRVAL_Q2          => '0',
					SRVAL_Q3          => '0',
					SRVAL_Q4          => '0'
				)
	PORT MAP (
				 O            => OPEN,
				 Q1           => fco_par(0),
				 Q2           => fco_par(1),
				 Q3           => fco_par(2),
				 Q4           => fco_par(3),
				 Q5           => fco_par(4),
				 Q6           => fco_par(5),
				 Q7           => fco_par(6),
				 Q8           => fco_par(7),
				 SHIFTOUT1    => OPEN,
				 SHIFTOUT2    => OPEN,
				 BITSLIP      => bitslip_internal,
				 CE1          => '1',
				 CE2          => '1',
				 CLK          => adc_clk,
				 CLKB         => adc_clk_n,
				 CLKDIV       => adc_clk_div,
				 CLKDIVP      => '0',
				 D            => fco_p,
				 DDLY         => fco_dly,
				 DYNCLKDIVSEL => '0',
				 DYNCLKSEL    => '0',
				 OCLK         => '0',
				 OCLKB        => '0',
				 OFB          => '0',
				 RST          => not reset,
				 SHIFTIN1     => '0',
				 SHIFTIN2     => '0'
			 );

	FCO_buf : IBUFDS_LVDS_25
	PORT MAP (
				 O  => fco_p,
				 I  => LVDS_ADC_FCO_P,
				 IB => LVDS_ADC_FCO_N
			 );

	delay_ctrl : IDELAYCTRL
	PORT MAP (
				 RDY    => iDelayRdy,
				 REFCLK => clk200MHz,
				 RST    => reset 
			 );

	FCO_delay : IDELAYE2
	GENERIC MAP (
					CINVCTRL_SEL          => "FALSE",
					DELAY_SRC             => "IDATAIN",
					HIGH_PERFORMANCE_MODE => "FALSE",
					IDELAY_TYPE           => "VARIABLE",
					IDELAY_VALUE          => 0,
					PIPE_SEL              => "FALSE",
					REFCLK_FREQUENCY      => 200.0,
					SIGNAL_PATTERN        => "CLOCK"
				)
	PORT MAP (
				 CNTVALUEOUT => OPEN,
				 DATAOUT     => fco_dly,
				 C           => adc_clk_div,
				 CE          => '0',
				 CINVCTRL    => '0',
				 CNTVALUEIN  => CNTVALUEIN,
				 DATAIN      => '0',
				 IDATAIN     => fco_p,
				 INC         => '0',
				 LD          => '0',
				 LDPIPEEN    => '0',
				 REGRST      => '0'
			 );

	FCO_Alignment : adc_fco_align_ctrl
	PORT MAP (
				 adc_clk_div     => adc_clk_div,
				 fco_serdes      => fco_serdes,
				 bitslip         => bitslip_internal,
				 serdes_ce       => serdes_ce,
				 serdes_rst      => serdes_rst
			 );

	bitslip <= bitslip_internal;

END ARCHITECTURE struct;
