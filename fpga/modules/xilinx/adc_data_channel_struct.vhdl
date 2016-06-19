LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.NUMERIC_STD.all;
LIBRARY unisim;
USE unisim.vcomponents.all;

ENTITY adc_data_channel IS
	PORT( 
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

END ENTITY adc_data_channel ;

ARCHITECTURE struct OF adc_data_channel IS


  -- Internal signal declarations
	SIGNAL D0_delay_const_0 : std_logic_vector(4 DOWNTO 0);
	SIGNAL adc_clk_n        : std_logic;
	SIGNAL adc_d0           : std_logic;
	SIGNAL adc_d0_dly       : std_logic;
	SIGNAL adc_d1           : std_logic;
	SIGNAL adc_d1_dly       : std_logic;
	SIGNAL d0_n_serdes_low  : std_logic;
	SIGNAL d0_p_serdes_low  : std_logic;
	SIGNAL d0_par           : std_logic_vector(7 DOWNTO 0);
	SIGNAL d1_par           : std_logic_vector(7 DOWNTO 0);


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


BEGIN
	adc_data <= d1_par & d0_par;

	d0_n_serdes_low <= '0';
	d0_p_serdes_low <= '0';
	D0_delay_const_0 <= (OTHERS => '0');
	D0_delay_const_0 <= (OTHERS => '0');
	adc_clk_n <= NOT(adc_clk);

	ISERDESE2_d0 : ISERDESE2
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
				 Q1           => d0_par(0),
				 Q2           => d0_par(1),
				 Q3           => d0_par(2),
				 Q4           => d0_par(3),
				 Q5           => d0_par(4),
				 Q6           => d0_par(5),
				 Q7           => d0_par(6),
				 Q8           => d0_par(7),
				 SHIFTOUT1    => OPEN,
				 SHIFTOUT2    => OPEN,
				 BITSLIP      => bitslip,
				 CE1          => '1',
				 CE2          => '1',
				 CLK          => adc_clk,
				 CLKB         => adc_clk_n,
				 CLKDIV       => adc_clk_div,
				 CLKDIVP      => d0_p_serdes_low,
				 D            => adc_d0,
				 DDLY         => adc_d0_dly,
				 DYNCLKDIVSEL => d0_p_serdes_low,
				 DYNCLKSEL    => d0_p_serdes_low,
				 OCLK         => d0_p_serdes_low,
				 OCLKB        => d0_p_serdes_low,
				 OFB          => d0_p_serdes_low,
				 RST          => not reset,
				 SHIFTIN1     => d0_p_serdes_low,
				 SHIFTIN2     => d0_p_serdes_low
			 );
	ISERDESE2_d1 : ISERDESE2
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
				 Q1           => d1_par(0),
				 Q2           => d1_par(1),
				 Q3           => d1_par(2),
				 Q4           => d1_par(3),
				 Q5           => d1_par(4),
				 Q6           => d1_par(5),
				 Q7           => d1_par(6),
				 Q8           => d1_par(7),
				 SHIFTOUT1    => OPEN,
				 SHIFTOUT2    => OPEN,
				 BITSLIP      => bitslip,
				 CE1          => '1', 
				 CE2          => '1',
				 CLK          => adc_clk,
				 CLKB         => adc_clk_n,
				 CLKDIV       => adc_clk_div,
				 CLKDIVP      => d0_n_serdes_low,
				 D            => adc_d1,
				 DDLY         => adc_d1_dly,
				 DYNCLKDIVSEL => d0_n_serdes_low,
				 DYNCLKSEL    => d0_n_serdes_low,
				 OCLK         => d0_n_serdes_low,
				 OCLKB        => d0_n_serdes_low,
				 OFB          => d0_n_serdes_low,
				 RST          => not reset,
				 SHIFTIN1     => d0_n_serdes_low,
				 SHIFTIN2     => d0_n_serdes_low
			 );
	D0_buf : IBUFDS_LVDS_25
	PORT MAP (
				 O  => adc_d0,
				 I  => LVDS_ADC_D0_P,
				 IB => LVDS_ADC_D0_N
			 );
	D1_buf : IBUFDS_LVDS_25
	PORT MAP (
				 O  => adc_d1,
				 I  => LVDS_ADC_D1_P,
				 IB => LVDS_ADC_D1_N
			 );
	D0_delay : IDELAYE2
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
				 DATAOUT     => adc_d0_dly,
				 C           => adc_clk_div,
				 CE          => D0_delay_const_0(0),
				 CINVCTRL    => D0_delay_const_0(0),
				 CNTVALUEIN  => D0_delay_const_0,
				 DATAIN      => D0_delay_const_0(0),
				 IDATAIN     => adc_d0,
				 INC         => D0_delay_const_0(0),
				 LD          => D0_delay_const_0(0),
				 LDPIPEEN    => D0_delay_const_0(0),
				 REGRST      => D0_delay_const_0(0)
			 );
	D1_delay : IDELAYE2
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
				 DATAOUT     => adc_d1_dly,
				 C           => adc_clk_div,
				 CE          => D0_delay_const_0(0),
				 CINVCTRL    => D0_delay_const_0(0),
				 CNTVALUEIN  => D0_delay_const_0,
				 DATAIN      => D0_delay_const_0(0),
				 IDATAIN     => adc_d1,
				 INC         => D0_delay_const_0(0),
				 LD          => D0_delay_const_0(0),
				 LDPIPEEN    => D0_delay_const_0(0),
				 REGRST      => D0_delay_const_0(0)
			 );

END ARCHITECTURE struct;
