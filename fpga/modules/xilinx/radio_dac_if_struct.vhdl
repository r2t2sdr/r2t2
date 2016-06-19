LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.NUMERIC_STD.all;
LIBRARY unisim;
USE unisim.vcomponents.all;

ENTITY radio_dac_if IS
  PORT( 
    DCLKIO              : IN   std_logic;
    S_AXIS_DAC_A_tdata  : IN   std_logic_vector (15 DOWNTO 0);
    S_AXIS_DAC_A_tvalid : IN   std_logic;
    S_AXIS_DAC_B_tdata  : IN   std_logic_vector (15 DOWNTO 0);
    S_AXIS_DAC_B_tvalid : IN   std_logic;
    clksys              : IN   std_logic;
  	ebuf_rst            : IN   std_logic;
	clk_ioctrl_200MHz   : IN   std_logic;
    DB                  : OUT  std_logic_vector (13 DOWNTO 0)
  );

END ENTITY radio_dac_if;

ARCHITECTURE struct OF radio_dac_if IS


  SIGNAL dac_clk      : std_logic;
  SIGNAL data_I       : std_logic_vector(13 DOWNTO 0);
  SIGNAL data_Q       : std_logic_vector(13 DOWNTO 0);

  -- Component Declarations
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

  COMPONENT ODDR
  GENERIC (
    DDR_CLK_EDGE : string := "OPPOSITE_EDGE";
    INIT         : bit    := '0';
    SRTYPE       : string := "SYNC"
  );
  PORT (
    C  : IN     std_ulogic;
    CE : IN     std_ulogic;
    D1 : IN     std_ulogic;
    D2 : IN     std_ulogic;
    R  : IN     std_ulogic  := 'L';
    S  : IN     std_ulogic  := 'L';
    Q  : OUT    std_ulogic
  );
  END COMPONENT ODDR;



BEGIN

  dac_clk_delay : IDELAYE2
    GENERIC MAP (
      CINVCTRL_SEL          => "FALSE",
      DELAY_SRC             => "IDATAIN",
      HIGH_PERFORMANCE_MODE => "FALSE",
      IDELAY_TYPE           => "FIXED",
      IDELAY_VALUE          => 10,        --  25:  125MHz -> 8ns/4/78ps
      PIPE_SEL              => "FALSE",
      REFCLK_FREQUENCY      => 200.0,
      SIGNAL_PATTERN        => "CLOCK"
    )
    PORT MAP (
      CNTVALUEOUT => OPEN,
      DATAOUT     => dac_clk,
      C           => '0',
      CE          => '0',
      CINVCTRL    => '0',
      CNTVALUEIN  => (others => '0'),
      DATAIN      => '0',
      IDATAIN     => DCLKIO, 
      INC         => '0',
      LD          => '0',
      LDPIPEEN    => '0',
      REGRST      => '0'
    );


  DAC_DDRs1: FOR i IN 0 TO 13 GENERATE

  BEGIN
    DAC_d : ODDR
      GENERIC MAP (
        DDR_CLK_EDGE => "SAME_EDGE",
        INIT         => '0',
        SRTYPE       => "SYNC"
      )
      PORT MAP (
        Q  => DB(i),
        C  => dac_clk,
        CE => '1', 
        D1 => data_Q(i),
        D2 => data_I(i),
        R  => not ebuf_rst,
        S  => '0'
      );
  END GENERATE DAC_DDRs1;

	read_data : PROCESS(clksys)
	BEGIN
		IF clksys'EVENT AND clksys = '1' THEN
			if S_AXIS_DAC_A_tvalid = '1' then
				data_I <= S_AXIS_DAC_A_tdata(15 downto 2);
			else
				data_I <= (others => '0');
			end if;
			if S_AXIS_DAC_B_tvalid = '1' then
				data_Q <= S_AXIS_DAC_B_tdata(15 downto 2);
			else
				data_Q <= (others => '0');
			end if;
		end if;
	END PROCESS;

END ARCHITECTURE struct;
