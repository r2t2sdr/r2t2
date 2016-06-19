library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.all;

entity top is
Port ( 
	M_SDA   : inout STD_LOGIC;
	M_SCL   : inout STD_LOGIC;
	PA_ON   : inout STD_LOGIC;
	P_RESET : out STD_LOGIC;
	EXT_SDA : inout STD_LOGIC;
	EXT_SCL : inout STD_LOGIC;

	DDR_addr : inout STD_LOGIC_VECTOR ( 14 downto 0 );
	DDR_ba : inout STD_LOGIC_VECTOR ( 2 downto 0 );
	DDR_cas_n : inout STD_LOGIC;
	DDR_ck_n : inout STD_LOGIC;
	DDR_ck_p : inout STD_LOGIC;
	DDR_cke : inout STD_LOGIC;
	DDR_cs_n : inout STD_LOGIC;
	DDR_dm : inout STD_LOGIC_VECTOR ( 3 downto 0 );
	DDR_dq : inout STD_LOGIC_VECTOR ( 31 downto 0 );
	DDR_dqs_n : inout STD_LOGIC_VECTOR ( 3 downto 0 );
	DDR_dqs_p : inout STD_LOGIC_VECTOR ( 3 downto 0 );
	DDR_odt : inout STD_LOGIC;
	DDR_ras_n : inout STD_LOGIC;
	DDR_reset_n : inout STD_LOGIC;
	DDR_we_n : inout STD_LOGIC;
	FIXED_IO_ddr_vrn : inout STD_LOGIC;
	FIXED_IO_ddr_vrp : inout STD_LOGIC;
	FIXED_IO_mio : inout STD_LOGIC_VECTOR ( 53 downto 0 );
	FIXED_IO_ps_clk : inout STD_LOGIC;
	FIXED_IO_ps_porb : inout STD_LOGIC;
	FIXED_IO_ps_srstb : inout STD_LOGIC;
	PL_PIN_K16 : in STD_LOGIC;
	PL_PIN_K19 : in STD_LOGIC;
	PL_PIN_K20 : out STD_LOGIC;
	PL_PIN_L16 : out STD_LOGIC;
	PL_PIN_M15 : in STD_LOGIC;
	PL_PIN_N15 : in STD_LOGIC;
	PL_PIN_N22 : out STD_LOGIC;
	PL_PIN_P16 : in STD_LOGIC;
	PL_PIN_P22 : in STD_LOGIC;

	LVDS_ADC_A_D0_N : in STD_LOGIC;
	LVDS_ADC_A_D0_P : in STD_LOGIC;
	LVDS_ADC_A_D1_N : in STD_LOGIC;
	LVDS_ADC_A_D1_P : in STD_LOGIC;
	LVDS_ADC_B_D0_N : in STD_LOGIC;
	LVDS_ADC_B_D0_P : in STD_LOGIC;
	LVDS_ADC_B_D1_N : in STD_LOGIC;
	LVDS_ADC_B_D1_P : in STD_LOGIC;
	LVDS_ADC_DCO_N  : in STD_LOGIC;
	LVDS_ADC_DCO_P  : in STD_LOGIC;
	LVDS_ADC_FCO_N  : in STD_LOGIC;
	LVDS_ADC_FCO_P  : in STD_LOGIC;
	
	PL_CLK_N        : in STD_LOGIC;
	PL_CLK_P        : in STD_LOGIC;

	DAC_RADIO_TX_CLK1   : in     std_logic;
	DAC_RADIO_TX_D      : out    std_logic_vector (13 DOWNTO 0);

	DA_SCLK 			: inout std_logic;
	DA_SDIO 			: inout std_logic;
	DA_CS 				: inout std_logic;

	ATT_A_LE 			: inout std_logic;
	ATT_B_LE 			: inout std_logic;
	ATT_CLK  			: inout std_logic;
	ATT_DAT  			: inout std_logic;
	
	PGA_LCH1 			: inout std_logic;
	PGA_LCH2 			: inout std_logic;
	PGA_SDI  			: inout std_logic;
	PGA_CLK  			: inout std_logic;
	
	ETH_LED2             : out std_logic;
	
	ENC1 				 : out std_logic;
	ENC2 				 : out std_logic;
	ENC3 				 : out std_logic;
	ENC4 				 : out std_logic;
	ENC5 				 : out std_logic;
	ENC6 				 : out std_logic;
	ENC7 				 : out std_logic;
	ENC8 				 : out std_logic;

    hdmi_out_clk         : out std_logic;      
    hdmi_out_vsync       : out std_logic;
    hdmi_out_hsync       : out std_logic;
    hdmi_out_de 		 : out std_logic;
    hdmi_out_data        : out std_logic_vector (11 downto 0);

    I2S_MCLK   		     : out std_logic;
    I2S_BCLK 		     : out std_logic;
    I2S_LRCLK 		     : out std_logic;
    I2S_SDATA_OUT 		 : out std_logic;
    I2S_SDATA_IN 		 : in  std_logic;

    SPDIF 		         : out std_logic;
    CEC_CLK              : out std_logic;
    LS_OE                : out std_logic;
    CT_HPD               : out std_logic

	);

	end entity top;

	architecture Behavioral of top is

	SIGNAL clk_idelayctrl   : std_logic;
	SIGNAL sys_clk          : std_logic;
	SIGNAL AXIS_ADC_A_tdata  : std_logic_vector (15 downto 0);
	SIGNAL AXIS_ADC_B_tdata  : std_logic_vector (15 downto 0);
	SIGNAL AXIS_DAC_tdata    : std_logic_vector (15 downto 0);
	SIGNAL AXIS_DAC_tvalid   : std_logic;
	SIGNAL gpio            : STD_LOGIC_VECTOR( 31 DOWNTO 0);
	SIGNAL phy_led         : STD_LOGIC;
	SIGNAL encline1 	   : STD_LOGIC;
	SIGNAL encline2 	   : STD_LOGIC;
	SIGNAL encline3 	   : STD_LOGIC;
	SIGNAL encline4 	   : STD_LOGIC;
	SIGNAL adc_lvds_reset  : STD_LOGIC;
    SIGNAL pl_clk          : std_logic;

component ps_wrapper 
	port (
	DB : out STD_LOGIC_VECTOR ( 13 downto 0 );
    DCLKIO : in STD_LOGIC;
    DDR_addr : inout STD_LOGIC_VECTOR ( 14 downto 0 );
    DDR_ba : inout STD_LOGIC_VECTOR ( 2 downto 0 );
    DDR_cas_n : inout STD_LOGIC;
    DDR_ck_n : inout STD_LOGIC;
    DDR_ck_p : inout STD_LOGIC;
    DDR_cke : inout STD_LOGIC;
    DDR_cs_n : inout STD_LOGIC;
    DDR_dm : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    DDR_dq : inout STD_LOGIC_VECTOR ( 31 downto 0 );
    DDR_dqs_n : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    DDR_dqs_p : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    DDR_odt : inout STD_LOGIC;
    DDR_ras_n : inout STD_LOGIC;
    DDR_reset_n : inout STD_LOGIC;
    DDR_we_n : inout STD_LOGIC;
    FIXED_IO_ddr_vrn : inout STD_LOGIC;
    FIXED_IO_ddr_vrp : inout STD_LOGIC;
    FIXED_IO_mio : inout STD_LOGIC_VECTOR ( 53 downto 0 );
    FIXED_IO_ps_clk : inout STD_LOGIC;
    FIXED_IO_ps_porb : inout STD_LOGIC;
    FIXED_IO_ps_srstb : inout STD_LOGIC;
    I2S_bclk : out STD_LOGIC_VECTOR ( 0 to 0 );
    I2S_lrclk : out STD_LOGIC_VECTOR ( 0 to 0 );
    I2S_sdata_in : in STD_LOGIC_VECTOR ( 0 to 0 );
    I2S_sdata_out : out STD_LOGIC_VECTOR ( 0 to 0 );
    LVDS_ADC_A_D0_N : in STD_LOGIC;
    LVDS_ADC_A_D0_P : in STD_LOGIC;
    LVDS_ADC_A_D1_N : in STD_LOGIC;
    LVDS_ADC_A_D1_P : in STD_LOGIC;
    LVDS_ADC_B_D0_N : in STD_LOGIC;
    LVDS_ADC_B_D0_P : in STD_LOGIC;
    LVDS_ADC_B_D1_N : in STD_LOGIC;
    LVDS_ADC_B_D1_P : in STD_LOGIC;
    LVDS_ADC_DCO_N : in STD_LOGIC;
    LVDS_ADC_DCO_P : in STD_LOGIC;
    LVDS_ADC_FCO_N : in STD_LOGIC;
    LVDS_ADC_FCO_P : in STD_LOGIC;
    PHY_LED0 : out STD_LOGIC;
    PHY_LED1 : out STD_LOGIC;
    PHY_LED2 : out STD_LOGIC;
    PL_PIN_K16 : in STD_LOGIC;
    PL_PIN_K19 : in STD_LOGIC;
    PL_PIN_K20 : out STD_LOGIC;
    PL_PIN_L16 : out STD_LOGIC;
    PL_PIN_M15 : in STD_LOGIC;
    PL_PIN_N15 : in STD_LOGIC;
    PL_PIN_N22 : out STD_LOGIC;
    PL_PIN_P16 : in STD_LOGIC;
    PL_PIN_P22 : in STD_LOGIC;
    clk_idelayctrl : out STD_LOGIC;
    gpio_tri_io : inout STD_LOGIC_VECTOR ( 31 downto 0 );
    hdmi_out_data : out STD_LOGIC_VECTOR ( 11 downto 0 );
    hdmi_out_de : out STD_LOGIC;
    hdmi_out_hsync : out STD_LOGIC;
    hdmi_out_vsync : out STD_LOGIC;
    hdmi_out_clk : out STD_LOGIC;
    i2s_mdk : out STD_LOGIC;
    iic_0_scl_io : inout STD_LOGIC;
    iic_0_sda_io : inout STD_LOGIC;
    pl_clk : in STD_LOGIC;
    clk_12mhz : out STD_LOGIC;
    sys_clk : out STD_LOGIC
);
end component ps_wrapper;

component IBUFDS_LVDS_25
  PORT (
    I  : IN     std_ulogic;
    IB : IN     std_ulogic;
    O  : OUT    std_ulogic
  );
end component IBUFDS_LVDS_25;

begin

iPS : ps_wrapper 	
	PORT MAP (
	DB         => DAC_RADIO_TX_D,
	DCLKIO     => DAC_RADIO_TX_CLK1,
	DDR_addr   => DDR_addr,
	DDR_ba     => DDR_ba,
	DDR_cas_n  => DDR_cas_n,
	DDR_ck_n   => DDR_ck_n,
	DDR_ck_p   => DDR_ck_p,
	DDR_cke    => DDR_cke,
	DDR_cs_n   => DDR_cs_n,
	DDR_dm     => DDR_dm,
	DDR_dq     => DDR_dq,
	DDR_dqs_n  => DDR_dqs_n,
	DDR_dqs_p  => DDR_dqs_p,
	DDR_odt    => DDR_odt,
	DDR_ras_n  => DDR_ras_n,
	DDR_reset_n => DDR_reset_n,
	DDR_we_n    => DDR_we_n,
	FIXED_IO_ddr_vrn => FIXED_IO_ddr_vrn,
	FIXED_IO_ddr_vrp => FIXED_IO_ddr_vrp,
	FIXED_IO_mio     => FIXED_IO_mio,
	FIXED_IO_ps_clk  => FIXED_IO_ps_clk,
	FIXED_IO_ps_porb => FIXED_IO_ps_porb,
	FIXED_IO_ps_srstb => FIXED_IO_ps_srstb,
    I2S_bclk(0)       => I2S_BCLK,
    I2S_lrclk(0)      => I2S_LRCLK,
    I2S_sdata_in(0)   => I2S_SDATA_IN,
    I2S_sdata_out(0)  => I2S_SDATA_OUT,
	LVDS_ADC_A_D0_N => LVDS_ADC_A_D0_N,
	LVDS_ADC_A_D0_P => LVDS_ADC_A_D0_P,
	LVDS_ADC_A_D1_N => LVDS_ADC_A_D1_N,
	LVDS_ADC_A_D1_P => LVDS_ADC_A_D1_P,
	LVDS_ADC_B_D0_N => LVDS_ADC_B_D0_N,
	LVDS_ADC_B_D0_P => LVDS_ADC_B_D0_P,
	LVDS_ADC_B_D1_N => LVDS_ADC_B_D1_N,
	LVDS_ADC_B_D1_P => LVDS_ADC_B_D1_P,
	LVDS_ADC_DCO_N  => LVDS_ADC_DCO_N,
	LVDS_ADC_DCO_P  => LVDS_ADC_DCO_P,
	LVDS_ADC_FCO_N  => LVDS_ADC_FCO_N,
	LVDS_ADC_FCO_P  => LVDS_ADC_FCO_P,
	PHY_LED0 => OPEN,
	PHY_LED1 => OPEN,
	PHY_LED2 => phy_led,
	PL_PIN_K16 => PL_PIN_K16,
	PL_PIN_K19 => PL_PIN_K19,
	PL_PIN_K20 => PL_PIN_K20,
	PL_PIN_L16 => PL_PIN_L16,
	PL_PIN_M15 => PL_PIN_M15,
	PL_PIN_N15 => PL_PIN_N15,
	PL_PIN_N22 => PL_PIN_N22,
	PL_PIN_P16 => PL_PIN_P16,
	PL_PIN_P22 => PL_PIN_P22,
	clk_idelayctrl => clk_idelayctrl,
	gpio_tri_io    => gpio,
    hdmi_out_data  => hdmi_out_data,
    hdmi_out_de    => hdmi_out_de,
    hdmi_out_hsync => hdmi_out_hsync,
    hdmi_out_vsync => hdmi_out_vsync,
    hdmi_out_clk   => hdmi_out_clk,
    i2s_mdk        => open, 
	iic_0_scl_io   => M_SCL,
	iic_0_sda_io   => M_SDA,
	pl_clk         => pl_clk,
    clk_12mhz      => CEC_CLK,
	sys_clk        => sys_clk
);

DCO_buf : IBUFDS_LVDS_25
PORT MAP (
	  O  => pl_clk,
	  I  => PL_CLK_P,
	  IB => PL_CLK_N
);

PA_ON   <= gpio(11);
P_RESET <= '1';
LS_OE   <= '1';
CT_HPD  <= '1';
SPDIF   <= '0';

DA_SCLK <= gpio(0);
DA_SDIO <= gpio(1);
DA_CS   <= gpio(2);

ATT_A_LE <= gpio(3);
ATT_B_LE <= gpio(4);
ATT_CLK  <= gpio(5);
ATT_DAT  <= gpio(6);

PGA_LCH1 <= gpio(7);
PGA_LCH2 <= gpio(8);
PGA_SDI  <= gpio(9);
PGA_CLK  <= gpio(10);

ETH_LED2 <= phy_led;

ENC1 <= encline1;
ENC2 <= encline2;
ENC3 <= encline3;
ENC4 <= encline4;



end Behavioral;
