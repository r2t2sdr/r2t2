------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2012 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.wdl_synth.symbol
--
-- GENERATED:
--  Tool: Mentor Graphics HDL Designer(TM) 2012.1 (Build 6)
--  Date: 16:21:33 23.01.2015 
--
------------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.NUMERIC_STD.all;

ENTITY wdl_synth IS
  PORT( 
    CLK_RMII_REF        : IN     STD_LOGIC;
    CTRL_RADIO_IRQ0     : IN     std_logic;
    CTRL_RADIO_IRQ1     : IN     std_logic;
    CTRL_RADIO_PWR_GOOD : IN     std_logic;
    DAC_RADIO_TX_CLK1   : IN     std_logic;
    IRQ_PHY1            : IN     STD_LOGIC;
    IRQ_PHY2            : IN     STD_LOGIC;
    LVDS_ADC_A_D0_N     : IN     std_logic;
    LVDS_ADC_A_D0_P     : IN     std_logic;
    LVDS_ADC_A_D1_N     : IN     std_logic;
    LVDS_ADC_A_D1_P     : IN     std_logic;
    LVDS_ADC_B_D0_N     : IN     std_logic;
    LVDS_ADC_B_D0_P     : IN     std_logic;
    LVDS_ADC_B_D1_N     : IN     std_logic;
    LVDS_ADC_B_D1_P     : IN     std_logic;
    LVDS_ADC_C_D0_N     : IN     std_logic;
    LVDS_ADC_C_D0_P     : IN     std_logic;
    LVDS_ADC_C_D1_N     : IN     std_logic;
    LVDS_ADC_C_D1_P     : IN     std_logic;
    LVDS_ADC_DCO_N      : IN     std_logic;
    LVDS_ADC_DCO_P      : IN     std_logic;
    LVDS_ADC_D_D0_N     : IN     std_logic;
    LVDS_ADC_D_D0_P     : IN     std_logic;
    LVDS_ADC_D_D1_N     : IN     std_logic;
    LVDS_ADC_D_D1_P     : IN     std_logic;
    LVDS_ADC_FCO_N      : IN     std_logic;
    LVDS_ADC_FCO_P      : IN     std_logic;
    RMII1_CRS_DV        : IN     STD_LOGIC;
    RMII1_RX_D0         : IN     std_logic;
    RMII1_RX_D1         : IN     std_logic;
    RMII1_RX_ER         : IN     STD_LOGIC;
    RMII2_CRS_DV        : IN     STD_LOGIC;
    RMII2_RX_D0         : IN     std_logic;
    RMII2_RX_D1         : IN     std_logic;
    RMII2_RX_ER         : IN     STD_LOGIC;
    RS232_CTS           : IN     std_logic;
    RS232_RXD           : IN     std_logic;
    RS485_RXD           : IN     std_logic;
    SPI_MISO            : IN     std_logic;
    SPI_RADIO_MISO      : IN     STD_LOGIC;
    CTRL_RADIO_PTT      : OUT    std_logic;
    CTRL_RADIO_PWR_EN   : OUT    std_logic;
    CTRL_RADIO_RST      : OUT    std_logic;
    DAC_RADIO_TX_D      : OUT    std_logic_vector (11 DOWNTO 0);
    FREQ_DEC            : OUT    std_logic;
    FREQ_INC            : OUT    std_logic;
    LED_PL1_G_1V8       : OUT    std_logic;
    LED_PL1_R_1V8       : OUT    std_logic;
    LED_PL2_G_1V8       : OUT    std_logic;
    LED_PL2_R_1V8       : OUT    std_logic;
    MDIO_CLK            : OUT    STD_LOGIC;
    RMII1_TX_D0         : OUT    std_logic;
    RMII1_TX_D1         : OUT    std_logic;
    RMII1_TX_EN         : OUT    STD_LOGIC;
    RMII2_TX_D0         : OUT    std_logic;
    RMII2_TX_D1         : OUT    std_logic;
    RMII2_TX_EN         : OUT    STD_LOGIC;
    RS232_RTS           : OUT    std_logic;
    RS232_TXD           : OUT    std_logic;
    RS485_DE            : OUT    std_logic;
    RS485_RE            : OUT    std_logic;
    RS485_TXD           : OUT    std_logic;
    SPI_ADC_DAC_CLK_L   : OUT    std_logic;
    SPI_ADC_DAC_CS_ADC  : OUT    std_logic;
    SPI_ADC_DAC_CS_DAC  : OUT    std_logic;
    SPI_CLK_L           : OUT    std_logic;
    SPI_CS_UI           : OUT    std_logic;
    SPI_MOSI            : OUT    std_logic;
    SPI_RADIO_CLK_L     : OUT    STD_LOGIC;
    SPI_RADIO_MOSI      : OUT    STD_LOGIC;
    SPI_RADIO_SEL       : OUT    std_logic_vector (2 DOWNTO 0);
    CTRL_RADIO_RES0     : INOUT  std_logic;
    CTRL_RADIO_RES1     : INOUT  std_logic;
    DDR_addr            : INOUT  STD_LOGIC_VECTOR ( 14 DOWNTO 0 );
    DDR_ba              : INOUT  STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    DDR_cas_n           : INOUT  STD_LOGIC;
    DDR_ck_n            : INOUT  STD_LOGIC;
    DDR_ck_p            : INOUT  STD_LOGIC;
    DDR_cke             : INOUT  STD_LOGIC;
    DDR_cs_n            : INOUT  STD_LOGIC;
    DDR_dm              : INOUT  STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    DDR_dq              : INOUT  STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    DDR_dqs_n           : INOUT  STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    DDR_dqs_p           : INOUT  STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    DDR_odt             : INOUT  STD_LOGIC;
    DDR_ras_n           : INOUT  STD_LOGIC;
    DDR_reset_n         : INOUT  STD_LOGIC;
    DDR_we_n            : INOUT  STD_LOGIC;
    FIXED_IO_ddr_vrn    : INOUT  STD_LOGIC;
    FIXED_IO_ddr_vrp    : INOUT  STD_LOGIC;
    FIXED_IO_mio        : INOUT  STD_LOGIC_VECTOR ( 53 DOWNTO 0 );
    FIXED_IO_ps_clk     : INOUT  STD_LOGIC;
    FIXED_IO_ps_porb    : INOUT  STD_LOGIC;
    FIXED_IO_ps_srstb   : INOUT  STD_LOGIC;
    MDIO_DAT            : INOUT  STD_LOGIC;
    SPI_ADC_DAC_MIO     : INOUT  std_logic
  );

-- Declarations

END ENTITY wdl_synth ;
------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2012 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.wdl_synth.struct
--
-- GENERATED:
--  Tool: Mentor Graphics HDL Designer(TM) 2012.1 (Build 6)
--  Date: 16:21:34 23.01.2015 
--
------------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.NUMERIC_STD.all;
LIBRARY wdl_lib;
USE wdl_lib.wdl_def.all;

LIBRARY wdl_xilinx;

ARCHITECTURE struct OF wdl_synth IS

  -- Architecture declarations

  -- Internal signal declarations
  SIGNAL ARESETN               : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXIS_ADC_A            : axi_stream_adc_t;
  SIGNAL AXIS_ADC_B            : axi_stream_adc_t;
  SIGNAL AXIS_ADC_C            : axi_stream_adc_t;
  SIGNAL AXIS_ADC_D            : axi_stream_adc_t;
  SIGNAL AXIS_DAC              : axi_stream_dac_t;
  SIGNAL AXI_OCB_Core0_araddr  : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL AXI_OCB_Core0_arprot  : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL AXI_OCB_Core0_arready : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core0_arvalid : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core0_awaddr  : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL AXI_OCB_Core0_awprot  : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL AXI_OCB_Core0_awready : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core0_awvalid : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core0_bready  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core0_bresp   : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL AXI_OCB_Core0_bvalid  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core0_rdata   : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL AXI_OCB_Core0_rready  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core0_rresp   : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL AXI_OCB_Core0_rvalid  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core0_wdata   : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL AXI_OCB_Core0_wready  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core0_wstrb   : STD_LOGIC_VECTOR( 3 DOWNTO 0 );
  SIGNAL AXI_OCB_Core0_wvalid  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core1_araddr  : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL AXI_OCB_Core1_arprot  : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL AXI_OCB_Core1_arready : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core1_arvalid : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core1_awaddr  : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL AXI_OCB_Core1_awprot  : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL AXI_OCB_Core1_awready : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core1_awvalid : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core1_bready  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core1_bresp   : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL AXI_OCB_Core1_bvalid  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core1_rdata   : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL AXI_OCB_Core1_rready  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core1_rresp   : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL AXI_OCB_Core1_rvalid  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core1_wdata   : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL AXI_OCB_Core1_wready  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL AXI_OCB_Core1_wstrb   : STD_LOGIC_VECTOR( 3 DOWNTO 0 );
  SIGNAL AXI_OCB_Core1_wvalid  : STD_LOGIC_VECTOR( 0 TO 0 );
  SIGNAL Core0_nFIQ            : STD_LOGIC;
  SIGNAL Core1_nFIQ            : STD_LOGIC;
  SIGNAL M_AXIS_HP0_tdata      : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL M_AXIS_HP0_tid        : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL M_AXIS_HP0_tkeep      : STD_LOGIC_VECTOR( 3 DOWNTO 0 );
  SIGNAL M_AXIS_HP0_tlast      : STD_LOGIC;
  SIGNAL M_AXIS_HP0_tvalid     : STD_LOGIC;
  SIGNAL RADIO_GPO             : STD_LOGIC_VECTOR( 7 DOWNTO 0 );
  SIGNAL RMII1_RX_D            : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL RMII1_TX_D            : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL RMII2_RX_D            : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL RMII2_TX_D            : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL SPI0_MISO             : STD_LOGIC;
  SIGNAL SPI0_MOSI             : STD_LOGIC;
  SIGNAL SPI0_SCLK             : STD_LOGIC;
  SIGNAL SPI0_SS               : std_logic_vector(2 DOWNTO 0);
  SIGNAL SPI_RADIO_SEL0        : STD_LOGIC;
  SIGNAL SPI_RADIO_SEL1        : STD_LOGIC;
  SIGNAL SPI_RADIO_SEL2        : STD_LOGIC;
  SIGNAL S_AXIS_HP0_tdata      : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL S_AXIS_HP0_tid        : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL S_AXIS_HP0_tkeep      : STD_LOGIC_VECTOR( 3 DOWNTO 0 );
  SIGNAL S_AXIS_HP0_tlast      : STD_LOGIC;
  SIGNAL S_AXIS_HP0_tvalid     : STD_LOGIC;
  SIGNAL S_AXI_ACP_araddr      : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL S_AXI_ACP_arburst     : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL S_AXI_ACP_arcache     : STD_LOGIC_VECTOR( 3 DOWNTO 0 );
  SIGNAL S_AXI_ACP_arid        : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL S_AXI_ACP_arlen       : STD_LOGIC_VECTOR( 3 DOWNTO 0 );
  SIGNAL S_AXI_ACP_arlock      : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL S_AXI_ACP_arprot      : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL S_AXI_ACP_arqos       : STD_LOGIC_VECTOR( 3 DOWNTO 0 );
  SIGNAL S_AXI_ACP_arready     : STD_LOGIC;
  SIGNAL S_AXI_ACP_arsize      : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL S_AXI_ACP_aruser      : STD_LOGIC_VECTOR( 4 DOWNTO 0 );
  SIGNAL S_AXI_ACP_arvalid     : STD_LOGIC;
  SIGNAL S_AXI_ACP_awaddr      : STD_LOGIC_VECTOR( 31 DOWNTO 0 );
  SIGNAL S_AXI_ACP_awburst     : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL S_AXI_ACP_awcache     : STD_LOGIC_VECTOR( 3 DOWNTO 0 );
  SIGNAL S_AXI_ACP_awid        : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL S_AXI_ACP_awlen       : STD_LOGIC_VECTOR( 3 DOWNTO 0 );
  SIGNAL S_AXI_ACP_awlock      : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL S_AXI_ACP_awprot      : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL S_AXI_ACP_awqos       : STD_LOGIC_VECTOR( 3 DOWNTO 0 );
  SIGNAL S_AXI_ACP_awready     : STD_LOGIC;
  SIGNAL S_AXI_ACP_awsize      : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL S_AXI_ACP_awuser      : STD_LOGIC_VECTOR( 4 DOWNTO 0 );
  SIGNAL S_AXI_ACP_awvalid     : STD_LOGIC;
  SIGNAL S_AXI_ACP_bid         : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL S_AXI_ACP_bready      : STD_LOGIC;
  SIGNAL S_AXI_ACP_bresp       : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL S_AXI_ACP_bvalid      : STD_LOGIC;
  SIGNAL S_AXI_ACP_rdata       : STD_LOGIC_VECTOR( 63 DOWNTO 0 );
  SIGNAL S_AXI_ACP_rid         : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL S_AXI_ACP_rlast       : STD_LOGIC;
  SIGNAL S_AXI_ACP_rready      : STD_LOGIC;
  SIGNAL S_AXI_ACP_rresp       : STD_LOGIC_VECTOR( 1 DOWNTO 0 );
  SIGNAL S_AXI_ACP_rvalid      : STD_LOGIC;
  SIGNAL S_AXI_ACP_wdata       : STD_LOGIC_VECTOR( 63 DOWNTO 0 );
  SIGNAL S_AXI_ACP_wid         : STD_LOGIC_VECTOR( 2 DOWNTO 0 );
  SIGNAL S_AXI_ACP_wlast       : STD_LOGIC;
  SIGNAL S_AXI_ACP_wready      : STD_LOGIC;
  SIGNAL S_AXI_ACP_wstrb       : STD_LOGIC_VECTOR( 7 DOWNTO 0 );
  SIGNAL S_AXI_ACP_wvalid      : STD_LOGIC;
  SIGNAL acp_miso              : axi3_acp_miso_t;
  SIGNAL acp_mosi              : axi3_acp_mosi_t;
  SIGNAL adc_enb               : std_logic_vector(3 DOWNTO 0);
  SIGNAL adc_status            : adc_status_t;
  SIGNAL clk_idelayctrl        : STD_LOGIC;
  SIGNAL clksys                : STD_LOGIC;
  SIGNAL dac_ebuf_rst          : std_logic;
  SIGNAL dac_ebuf_stat         : ebuf_status_t;
  SIGNAL leds                  : std_logic_vector(3 DOWNTO 0);
  SIGNAL m_axi_hp0_tready      : std_logic;
  SIGNAL m_axi_stream_hp0      : axi_stream_hp_t;
  SIGNAL oc_bus_core0          : oc_bus_t;
  SIGNAL oc_bus_core1          : oc_bus_t;
  SIGNAL ocb_rdata_core0       : ocb_data_t;
  SIGNAL ocb_rdata_core1       : ocb_data_t;
  SIGNAL radio_ctrl            : radio_ctrl_t;
  SIGNAL radio_stat            : radio_stat_t;
  SIGNAL s_axi_hp0_tready      : std_logic;
  SIGNAL s_axi_stream_hp0      : axi_stream_hp_t;
  SIGNAL spi_cfg_sdi           : std_logic;
  SIGNAL spi_config            : spi_cfg_t;
  SIGNAL spi_ui                : spi_ui_t;
  SIGNAL spi_ui_miso_loc       : std_logic;
  SIGNAL sreset                : std_logic_vector(0 DOWNTO 0);


  -- ModuleWare signal declarations(v1.9) for instance 'LED_splitter' of 'split'
  SIGNAL mw_LED_splittertemp_din : std_logic_vector(3 DOWNTO 0);

  -- ModuleWare signal declarations(v1.9) for instance 'RMII1_TXD_splitter' of 'split'
  SIGNAL mw_RMII1_TXD_splittertemp_din : std_logic_vector(1 DOWNTO 0);

  -- ModuleWare signal declarations(v1.9) for instance 'RMII1_TXD_splitter1' of 'split'
  SIGNAL mw_RMII1_TXD_splitter1temp_din : std_logic_vector(1 DOWNTO 0);

  -- Component Declarations
  COMPONENT wdl_top
  PORT (
    M_AXI_ACP_MISO   : IN     axi3_acp_miso_t ;
    RS232_CTS        : IN     std_logic ;
    RS232_RXD        : IN     std_logic ;
    RS485_RXD        : IN     std_logic ;
    SPI0_MOSI        : IN     STD_LOGIC ;
    SPI0_SCLK        : IN     STD_LOGIC ;
    SPI0_SS          : IN     std_logic_vector (2 DOWNTO 0);
    S_AXIS_ADC_A     : IN     axi_stream_adc_t ;
    S_AXIS_ADC_B     : IN     axi_stream_adc_t ;
    S_AXIS_ADC_C     : IN     axi_stream_adc_t ;
    S_AXIS_ADC_D     : IN     axi_stream_adc_t ;
    adc_status       : IN     adc_status_t ;
    clksys           : IN     STD_LOGIC ;
    dac_ebuf_stat    : IN     ebuf_status_t ;
    m_axi_stream_hp0 : IN     axi_stream_hp_t ;
    oc_bus_core0     : IN     oc_bus_t ;
    oc_bus_core1     : IN     oc_bus_t ;
    radio_stat       : IN     radio_stat_t ;
    s_axi_hp0_tready : IN     std_logic ;
    spi_cfg_sdi      : IN     std_logic ;
    spi_ui_miso      : IN     std_logic ;
    sreset           : IN     std_logic ;
    Core0_nFIQ       : OUT    STD_LOGIC ;
    Core1_nFIQ       : OUT    STD_LOGIC ;
    M_AXIS_DAC       : OUT    axi_stream_dac_t ;
    M_AXI_ACP_MOSI   : OUT    axi3_acp_mosi_t ;
    RS232_RTS        : OUT    std_logic ;
    RS232_TXD        : OUT    std_logic ;
    RS485_DE         : OUT    std_logic ;
    RS485_RE_n       : OUT    std_logic ;
    RS485_TXD        : OUT    std_logic ;
    SPI0_MISO        : OUT    STD_LOGIC ;
    adc_enb          : OUT    std_logic_vector (3 DOWNTO 0);
    dac_ebuf_rst     : OUT    std_logic ;
    led_1            : OUT    std_logic_vector (1 DOWNTO 0);
    led_2            : OUT    std_logic_vector (1 DOWNTO 0);
    m_axi_hp0_tready : OUT    std_logic ;
    ocb_rdata_core0  : OUT    ocb_data_t ;
    ocb_rdata_core1  : OUT    ocb_data_t ;
    radio_ctrl       : OUT    radio_ctrl_t ;
    s_axi_stream_hp0 : OUT    axi_stream_hp_t ;
    spi_config       : OUT    spi_cfg_t ;
    spi_ui           : OUT    spi_ui_t 
  );
  END COMPONENT wdl_top;
  COMPONENT AXI_OCB_bridge
  PORT (
    ACLK          : IN     std_logic;
    ARESETN       : IN     std_logic;
    S_AXI_ARADDR  : IN     std_logic_vector (31 DOWNTO 0);
    S_AXI_ARPROT  : IN     std_logic_vector (2 DOWNTO 0);
    S_AXI_ARVALID : IN     std_logic_vector (0 DOWNTO 0);
    S_AXI_AWADDR  : IN     std_logic_vector (31 DOWNTO 0);
    S_AXI_AWPROT  : IN     std_logic_vector (2 DOWNTO 0);
    S_AXI_AWVALID : IN     std_logic_vector (0 DOWNTO 0);
    S_AXI_BREADY  : IN     std_logic_vector (0 DOWNTO 0);
    S_AXI_RREADY  : IN     std_logic_vector (0 DOWNTO 0);
    S_AXI_WDATA   : IN     std_logic_vector (31 DOWNTO 0);
    S_AXI_WSTRB   : IN     std_logic_vector (3 DOWNTO 0);
    S_AXI_WVALID  : IN     std_logic_vector (0 DOWNTO 0);
    ocb_rdata     : IN     ocb_data_t;
    S_AXI_ARREADY : OUT    std_logic_vector (0 DOWNTO 0);
    S_AXI_AWREADY : OUT    std_logic_vector (0 DOWNTO 0);
    S_AXI_BRESP   : OUT    std_logic_vector (1 DOWNTO 0);
    S_AXI_BVALID  : OUT    std_logic_vector (0 DOWNTO 0);
    S_AXI_RDATA   : OUT    std_logic_vector (31 DOWNTO 0);
    S_AXI_RRESP   : OUT    std_logic_vector (1 DOWNTO 0);
    S_AXI_RVALID  : OUT    std_logic_vector (0 DOWNTO 0);
    S_AXI_WREADY  : OUT    std_logic_vector (0 DOWNTO 0);
    oc_bus        : OUT    oc_bus_t
  );
  END COMPONENT AXI_OCB_bridge;
  COMPONENT adc_lvds_if
  GENERIC (
    fco_swapped_g  : std_logic;
    data_swapped_g : std_logic_vector(7 DOWNTO 0) := "00000000"
  );
  PORT (
    LVDS_ADC_A_D0_N   : IN     std_logic ;
    LVDS_ADC_A_D0_P   : IN     std_logic ;
    LVDS_ADC_A_D1_N   : IN     std_logic ;
    LVDS_ADC_A_D1_P   : IN     std_logic ;
    LVDS_ADC_B_D0_N   : IN     std_logic ;
    LVDS_ADC_B_D0_P   : IN     std_logic ;
    LVDS_ADC_B_D1_N   : IN     std_logic ;
    LVDS_ADC_B_D1_P   : IN     std_logic ;
    LVDS_ADC_C_D0_N   : IN     std_logic ;
    LVDS_ADC_C_D0_P   : IN     std_logic ;
    LVDS_ADC_C_D1_N   : IN     std_logic ;
    LVDS_ADC_C_D1_P   : IN     std_logic ;
    LVDS_ADC_DCO_N    : IN     std_logic ;
    LVDS_ADC_DCO_P    : IN     std_logic ;
    LVDS_ADC_D_D0_N   : IN     std_logic ;
    LVDS_ADC_D_D0_P   : IN     std_logic ;
    LVDS_ADC_D_D1_N   : IN     std_logic ;
    LVDS_ADC_D_D1_P   : IN     std_logic ;
    LVDS_ADC_FCO_N    : IN     std_logic ;
    LVDS_ADC_FCO_P    : IN     std_logic ;
    adc_enb           : IN     std_logic_vector (3 DOWNTO 0);
    clk_ioctrl_200MHz : IN     std_logic ;
    clksys            : IN     std_logic ;
    sreset            : IN     std_logic ;
    M_AXIS_ACD_A      : OUT    axi_stream_adc_t ;
    M_AXIS_ACD_B      : OUT    axi_stream_adc_t ;
    M_AXIS_ACD_C      : OUT    axi_stream_adc_t ;
    M_AXIS_ACD_D      : OUT    axi_stream_adc_t ;
    adc_status        : OUT    adc_status_t 
  );
  END COMPONENT adc_lvds_if;
  COMPONENT radio_dac_if
  PORT (
    DCLKIO     : IN     std_logic ;
    S_AXIS_DAC : IN     axi_stream_dac_t ;
    clksys     : IN     std_logic ;
    ebuf_rst   : IN     std_logic ;
    DB         : OUT    std_logic_vector (11 DOWNTO 0);
    ebuf_stat  : OUT    ebuf_status_t 
  );
  END COMPONENT radio_dac_if;
  COMPONENT wdl_ps_wrapper
  PORT (
    ARESETN            : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    CLK_RMII_REF       : IN     STD_LOGIC ;
    Core0_nFIQ         : IN     STD_LOGIC ;
    Core1_nFIQ         : IN     STD_LOGIC ;
    DDR_addr           : INOUT  STD_LOGIC_VECTOR ( 14 DOWNTO 0 );
    DDR_ba             : INOUT  STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    DDR_cas_n          : INOUT  STD_LOGIC ;
    DDR_ck_n           : INOUT  STD_LOGIC ;
    DDR_ck_p           : INOUT  STD_LOGIC ;
    DDR_cke            : INOUT  STD_LOGIC ;
    DDR_cs_n           : INOUT  STD_LOGIC ;
    DDR_dm             : INOUT  STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    DDR_dq             : INOUT  STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    DDR_dqs_n          : INOUT  STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    DDR_dqs_p          : INOUT  STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    DDR_odt            : INOUT  STD_LOGIC ;
    DDR_ras_n          : INOUT  STD_LOGIC ;
    DDR_reset_n        : INOUT  STD_LOGIC ;
    DDR_we_n           : INOUT  STD_LOGIC ;
    FIXED_IO_ddr_vrn   : INOUT  STD_LOGIC ;
    FIXED_IO_ddr_vrp   : INOUT  STD_LOGIC ;
    FIXED_IO_mio       : INOUT  STD_LOGIC_VECTOR ( 53 DOWNTO 0 );
    FIXED_IO_ps_clk    : INOUT  STD_LOGIC ;
    FIXED_IO_ps_porb   : INOUT  STD_LOGIC ;
    FIXED_IO_ps_srstb  : INOUT  STD_LOGIC ;
    MDIO_mdc           : OUT    STD_LOGIC ;
    M_AXIS_HP0_tdata   : OUT    STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    M_AXIS_HP0_tid     : OUT    STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    M_AXIS_HP0_tkeep   : OUT    STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    M_AXIS_HP0_tlast   : OUT    STD_LOGIC ;
    M_AXIS_HP0_tready  : IN     STD_LOGIC ;
    M_AXIS_HP0_tvalid  : OUT    STD_LOGIC ;
    M_AXI_OCB0_araddr  : OUT    STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    M_AXI_OCB0_arprot  : OUT    STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    M_AXI_OCB0_arready : IN     STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB0_arvalid : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB0_awaddr  : OUT    STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    M_AXI_OCB0_awprot  : OUT    STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    M_AXI_OCB0_awready : IN     STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB0_awvalid : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB0_bready  : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB0_bresp   : IN     STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    M_AXI_OCB0_bvalid  : IN     STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB0_rdata   : IN     STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    M_AXI_OCB0_rready  : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB0_rresp   : IN     STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    M_AXI_OCB0_rvalid  : IN     STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB0_wdata   : OUT    STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    M_AXI_OCB0_wready  : IN     STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB0_wstrb   : OUT    STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    M_AXI_OCB0_wvalid  : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB1_araddr  : OUT    STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    M_AXI_OCB1_arprot  : OUT    STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    M_AXI_OCB1_arready : IN     STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB1_arvalid : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB1_awaddr  : OUT    STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    M_AXI_OCB1_awprot  : OUT    STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    M_AXI_OCB1_awready : IN     STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB1_awvalid : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB1_bready  : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB1_bresp   : IN     STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    M_AXI_OCB1_bvalid  : IN     STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB1_rdata   : IN     STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    M_AXI_OCB1_rready  : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB1_rresp   : IN     STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    M_AXI_OCB1_rvalid  : IN     STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB1_wdata   : OUT    STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    M_AXI_OCB1_wready  : IN     STD_LOGIC_VECTOR ( 0 TO 0 );
    M_AXI_OCB1_wstrb   : OUT    STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    M_AXI_OCB1_wvalid  : OUT    STD_LOGIC_VECTOR ( 0 TO 0 );
    PHY_IRQ_0          : IN     STD_LOGIC ;
    PHY_IRQ_1          : IN     STD_LOGIC ;
    RADIO_GPI          : IN     STD_LOGIC_VECTOR ( 7 DOWNTO 0 );
    RADIO_GPIO_T       : OUT    STD_LOGIC_VECTOR ( 7 DOWNTO 0 );
    RADIO_GPO          : OUT    STD_LOGIC_VECTOR ( 7 DOWNTO 0 );
    RMII0_crs_dv       : IN     STD_LOGIC ;
    RMII0_rx_er        : IN     STD_LOGIC ;
    RMII0_rxd          : IN     STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    RMII0_tx_en        : OUT    STD_LOGIC ;
    RMII0_txd          : OUT    STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    RMII1_crs_dv       : IN     STD_LOGIC ;
    RMII1_rx_er        : IN     STD_LOGIC ;
    RMII1_rxd          : IN     STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    RMII1_tx_en        : OUT    STD_LOGIC ;
    RMII1_txd          : OUT    STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    SPI0_MISO          : IN     STD_LOGIC ;
    SPI0_MOSI          : OUT    STD_LOGIC ;
    SPI0_SCLK          : OUT    STD_LOGIC ;
    SPI0_SS0           : OUT    STD_LOGIC ;
    SPI0_SS1           : OUT    STD_LOGIC ;
    SPI0_SS2           : OUT    STD_LOGIC ;
    SPI_RADIO_CLK_L    : OUT    STD_LOGIC ;
    SPI_RADIO_MISO     : IN     STD_LOGIC ;
    SPI_RADIO_MOSI     : OUT    STD_LOGIC ;
    SPI_RADIO_SEL0     : OUT    STD_LOGIC ;
    SPI_RADIO_SEL1     : OUT    STD_LOGIC ;
    SPI_RADIO_SEL2     : OUT    STD_LOGIC ;
    S_AXIS_HP0_tdata   : IN     STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    S_AXIS_HP0_tid     : IN     STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    S_AXIS_HP0_tkeep   : IN     STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    S_AXIS_HP0_tlast   : IN     STD_LOGIC ;
    S_AXIS_HP0_tready  : OUT    STD_LOGIC ;
    S_AXIS_HP0_tvalid  : IN     STD_LOGIC ;
    S_AXI_ACP_araddr   : IN     STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    S_AXI_ACP_arburst  : IN     STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    S_AXI_ACP_arcache  : IN     STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    S_AXI_ACP_arid     : IN     STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    S_AXI_ACP_arlen    : IN     STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    S_AXI_ACP_arlock   : IN     STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    S_AXI_ACP_arprot   : IN     STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    S_AXI_ACP_arqos    : IN     STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    S_AXI_ACP_arready  : OUT    STD_LOGIC ;
    S_AXI_ACP_arsize   : IN     STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    S_AXI_ACP_aruser   : IN     STD_LOGIC_VECTOR ( 4 DOWNTO 0 );
    S_AXI_ACP_arvalid  : IN     STD_LOGIC ;
    S_AXI_ACP_awaddr   : IN     STD_LOGIC_VECTOR ( 31 DOWNTO 0 );
    S_AXI_ACP_awburst  : IN     STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    S_AXI_ACP_awcache  : IN     STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    S_AXI_ACP_awid     : IN     STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    S_AXI_ACP_awlen    : IN     STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    S_AXI_ACP_awlock   : IN     STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    S_AXI_ACP_awprot   : IN     STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    S_AXI_ACP_awqos    : IN     STD_LOGIC_VECTOR ( 3 DOWNTO 0 );
    S_AXI_ACP_awready  : OUT    STD_LOGIC ;
    S_AXI_ACP_awsize   : IN     STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    S_AXI_ACP_awuser   : IN     STD_LOGIC_VECTOR ( 4 DOWNTO 0 );
    S_AXI_ACP_awvalid  : IN     STD_LOGIC ;
    S_AXI_ACP_bid      : OUT    STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    S_AXI_ACP_bready   : IN     STD_LOGIC ;
    S_AXI_ACP_bresp    : OUT    STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    S_AXI_ACP_bvalid   : OUT    STD_LOGIC ;
    S_AXI_ACP_rdata    : OUT    STD_LOGIC_VECTOR ( 63 DOWNTO 0 );
    S_AXI_ACP_rid      : OUT    STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    S_AXI_ACP_rlast    : OUT    STD_LOGIC ;
    S_AXI_ACP_rready   : IN     STD_LOGIC ;
    S_AXI_ACP_rresp    : OUT    STD_LOGIC_VECTOR ( 1 DOWNTO 0 );
    S_AXI_ACP_rvalid   : OUT    STD_LOGIC ;
    S_AXI_ACP_wdata    : IN     STD_LOGIC_VECTOR ( 63 DOWNTO 0 );
    S_AXI_ACP_wid      : IN     STD_LOGIC_VECTOR ( 2 DOWNTO 0 );
    S_AXI_ACP_wlast    : IN     STD_LOGIC ;
    S_AXI_ACP_wready   : OUT    STD_LOGIC ;
    S_AXI_ACP_wstrb    : IN     STD_LOGIC_VECTOR ( 7 DOWNTO 0 );
    S_AXI_ACP_wvalid   : IN     STD_LOGIC ;
    clk_idelayctrl     : OUT    STD_LOGIC ;
    clksys             : OUT    STD_LOGIC ;
    mdio_mdio_io       : INOUT  STD_LOGIC ;
    sreset             : OUT    STD_LOGIC_VECTOR ( 0 TO 0 )
  );
  END COMPONENT wdl_ps_wrapper;
  COMPONENT wdl_spi_cfg_if
  PORT (
    spi_config           : IN     spi_cfg_t ;
    SPI_ADC_DAC_CS_ADC_n : OUT    std_logic ;
    SPI_ADC_DAC_CS_DAC_n : OUT    std_logic ;
    SPI_ADC_DAC_SCLK     : OUT    std_logic ;
    spi_cfg_sdi          : OUT    std_logic ;
    SPI_ADC_DAC_MIO      : INOUT  std_logic 
  );
  END COMPONENT wdl_spi_cfg_if;
  COMPONENT wdl_spi_ui_if
  PORT (
    SPI_UI_MISO     : IN     std_logic ;
    spi_ui          : IN     spi_ui_t ;
    SPI_UI_CS_n     : OUT    std_logic ;
    SPI_UI_MOSI     : OUT    std_logic ;
    SPI_UI_SCLK     : OUT    std_logic ;
    spi_ui_miso_loc : OUT    std_logic 
  );
  END COMPONENT wdl_spi_ui_if;

  -- Optional embedded configurations
  -- synthesis translate_off
  FOR ALL : AXI_OCB_bridge USE ENTITY wdl_xilinx.AXI_OCB_bridge;
  FOR ALL : adc_lvds_if USE ENTITY wdl_xilinx.adc_lvds_if;
  FOR ALL : radio_dac_if USE ENTITY wdl_xilinx.radio_dac_if;
  FOR ALL : wdl_ps_wrapper USE ENTITY wdl_xilinx.wdl_ps_wrapper;
  FOR ALL : wdl_spi_cfg_if USE ENTITY wdl_xilinx.wdl_spi_cfg_if;
  FOR ALL : wdl_spi_ui_if USE ENTITY wdl_xilinx.wdl_spi_ui_if;
  FOR ALL : wdl_top USE ENTITY wdl_lib.wdl_top;
  -- synthesis translate_on


BEGIN
  -- Architecture concurrent statements
  -- HDL Embedded Text Block 1 AXI_HP0_stream_mapper
  --
  -- AXI_HP0_stream_mapper
  m_axi_stream_hp0.tid    <= M_AXIS_HP0_tid   ;
  m_axi_stream_hp0.tvalid <= M_AXIS_HP0_tvalid;
  m_axi_stream_hp0.tkeep  <= M_AXIS_HP0_tkeep ;
  m_axi_stream_hp0.tlast  <= M_AXIS_HP0_tlast ;
  m_axi_stream_hp0.tdata  <= M_AXIS_HP0_tdata ;
  --
  S_AXIS_HP0_tid    <= s_axi_stream_hp0.tid   ;
  S_AXIS_HP0_tvalid <= s_axi_stream_hp0.tvalid;
  S_AXIS_HP0_tkeep  <= s_axi_stream_hp0.tkeep ;
  S_AXIS_HP0_tlast  <= s_axi_stream_hp0.tlast ;
  S_AXIS_HP0_tdata  <= s_axi_stream_hp0.tdata ;

  -- HDL Embedded Text Block 2 AXI3_ACP_mapper
  --
  -- AXI3_ACP_mapper..
  -- write address channel
  S_AXI_ACP_awvalid <= acp_mosi.awvalid;
  S_AXI_ACP_awid    <= acp_mosi.awid   ;
  S_AXI_ACP_awprot  <= acp_mosi.awprot ;
  S_AXI_ACP_awlock  <= acp_mosi.awlock ;
  S_AXI_ACP_awlen   <= acp_mosi.awlen  ;
  S_AXI_ACP_awsize  <= acp_mosi.awsize ;
  S_AXI_ACP_awburst <= acp_mosi.awburst;
  S_AXI_ACP_awqos   <= acp_mosi.awqos  ;
  S_AXI_ACP_awcache <= acp_mosi.awcache;
  S_AXI_ACP_awaddr  <= acp_mosi.awaddr ;
  S_AXI_ACP_awuser  <= acp_mosi.awuser ;
  acp_miso.awready  <= S_AXI_ACP_awready;
  -- write data channel
  S_AXI_ACP_wvalid  <= acp_mosi.wvalid;
  S_AXI_ACP_wid     <= acp_mosi.wid   ;
  S_AXI_ACP_wlast   <= acp_mosi.wlast ;
  S_AXI_ACP_wstrb   <= acp_mosi.wstrb ;
  S_AXI_ACP_wdata   <= acp_mosi.wdata ;
  acp_miso.wready  <= S_AXI_ACP_wready;
  -- write response channel
  S_AXI_ACP_bready  <= acp_mosi.bready;
  acp_miso.bvalid   <= S_AXI_ACP_bvalid;
  acp_miso.bid      <= S_AXI_ACP_bid   ;
  acp_miso.bresp    <= S_AXI_ACP_bresp ;
  -- read address channel
  S_AXI_ACP_arvalid <= acp_mosi.arvalid;
  S_AXI_ACP_arid    <= acp_mosi.arid   ;
  S_AXI_ACP_arprot  <= acp_mosi.arprot ;
  S_AXI_ACP_arlock  <= acp_mosi.arlock ;
  S_AXI_ACP_arlen   <= acp_mosi.arlen  ;
  S_AXI_ACP_arsize  <= acp_mosi.arsize ;
  S_AXI_ACP_arburst <= acp_mosi.arburst;
  S_AXI_ACP_arqos   <= acp_mosi.arqos  ;
  S_AXI_ACP_arcache <= acp_mosi.arcache;
  S_AXI_ACP_araddr  <= acp_mosi.araddr ;
  S_AXI_ACP_aruser  <= acp_mosi.aruser ;
  acp_miso.arready  <= S_AXI_ACP_arready;
  -- read response channel
  S_AXI_ACP_rready  <= acp_mosi.rready;
  acp_miso.rvalid  <= S_AXI_ACP_rvalid;
  acp_miso.rid     <= S_AXI_ACP_rid   ;
  acp_miso.rlast   <= S_AXI_ACP_rlast ;
  acp_miso.rresp   <= S_AXI_ACP_rresp ;
  acp_miso.rdata   <= S_AXI_ACP_rdata ;

  -- HDL Embedded Text Block 3 radio_ctrl
  -- radio_ctrl
  FREQ_INC <= radio_ctrl.freq_inc;
  FREQ_DEC <= radio_ctrl.freq_dec;
  CTRL_RADIO_RST <= radio_ctrl.rst_n;
  CTRL_RADIO_PWR_EN <= radio_ctrl.pwr_en_n;
  CTRL_RADIO_PTT <= radio_ctrl.ptt;
  CTRL_RADIO_RES0 <= radio_ctrl.reserved(0) WHEN radio_ctrl.reserved_highZ(0) = '0' ELSE 'Z';
  CTRL_RADIO_RES1 <= radio_ctrl.reserved(1) WHEN radio_ctrl.reserved_highZ(1) = '0' ELSE 'Z';
  --
  radio_stat.pwr_good_n <= CTRL_RADIO_PWR_GOOD;
  radio_stat.irq_n      <= CTRL_RADIO_IRQ1 & CTRL_RADIO_IRQ0;
  radio_stat.reserved   <= CTRL_RADIO_RES1 & CTRL_RADIO_RES0;
  
  


  -- ModuleWare code(v1.9) for instance 'RMII2_RXD_merge' of 'merge'
  RMII2_RX_D <= RMII2_RX_D1 & RMII2_RX_D0;

  -- ModuleWare code(v1.9) for instance 'RMII2_RXD_merge1' of 'merge'
  RMII1_RX_D <= RMII1_RX_D1 & RMII1_RX_D0;

  -- ModuleWare code(v1.9) for instance 'SPI_RADIO_SEL_merge' of 'merge'
  SPI_RADIO_SEL <= SPI_RADIO_SEL2 & SPI_RADIO_SEL1 & SPI_RADIO_SEL0;

  -- ModuleWare code(v1.9) for instance 'LED_splitter' of 'split'
  mw_LED_splittertemp_din <= leds;
  led_splittercombo_proc: PROCESS (mw_LED_splittertemp_din)
  VARIABLE temp_din: std_logic_vector(3 DOWNTO 0);
  BEGIN
    temp_din := mw_LED_splittertemp_din(3 DOWNTO 0);
    LED_PL1_R_1V8 <= temp_din(0);
    LED_PL1_G_1V8 <= temp_din(1);
    LED_PL2_R_1V8 <= temp_din(2);
    LED_PL2_G_1V8 <= temp_din(3);
  END PROCESS led_splittercombo_proc;

  -- ModuleWare code(v1.9) for instance 'RMII1_TXD_splitter' of 'split'
  mw_RMII1_TXD_splittertemp_din <= RMII2_TX_D;
  rmii1_txd_splittercombo_proc: PROCESS (mw_RMII1_TXD_splittertemp_din)
  VARIABLE temp_din: std_logic_vector(1 DOWNTO 0);
  BEGIN
    temp_din := mw_RMII1_TXD_splittertemp_din(1 DOWNTO 0);
    RMII2_TX_D0 <= temp_din(0);
    RMII2_TX_D1 <= temp_din(1);
  END PROCESS rmii1_txd_splittercombo_proc;

  -- ModuleWare code(v1.9) for instance 'RMII1_TXD_splitter1' of 'split'
  mw_RMII1_TXD_splitter1temp_din <= RMII1_TX_D;
  rmii1_txd_splitter1combo_proc: PROCESS (mw_RMII1_TXD_splitter1temp_din)
  VARIABLE temp_din: std_logic_vector(1 DOWNTO 0);
  BEGIN
    temp_din := mw_RMII1_TXD_splitter1temp_din(1 DOWNTO 0);
    RMII1_TX_D0 <= temp_din(0);
    RMII1_TX_D1 <= temp_din(1);
  END PROCESS rmii1_txd_splitter1combo_proc;

  -- Instance port mappings.
  iTop : wdl_top
    PORT MAP (
      M_AXI_ACP_MISO   => acp_miso,
      RS232_CTS        => RS232_CTS,
      RS232_RXD        => RS232_RXD,
      RS485_RXD        => RS485_RXD,
      SPI0_MOSI        => SPI0_MOSI,
      SPI0_SCLK        => SPI0_SCLK,
      SPI0_SS          => SPI0_SS,
      S_AXIS_ADC_A     => AXIS_ADC_A,
      S_AXIS_ADC_B     => AXIS_ADC_B,
      S_AXIS_ADC_C     => AXIS_ADC_C,
      S_AXIS_ADC_D     => AXIS_ADC_D,
      adc_status       => adc_status,
      clksys           => clksys,
      dac_ebuf_stat    => dac_ebuf_stat,
      m_axi_stream_hp0 => m_axi_stream_hp0,
      oc_bus_core0     => oc_bus_core0,
      oc_bus_core1     => oc_bus_core1,
      radio_stat       => radio_stat,
      s_axi_hp0_tready => s_axi_hp0_tready,
      spi_cfg_sdi      => spi_cfg_sdi,
      spi_ui_miso      => spi_ui_miso_loc,
      sreset           => sreset(0),
      Core0_nFIQ       => Core1_nFIQ,
      Core1_nFIQ       => Core0_nFIQ,
      M_AXIS_DAC       => AXIS_DAC,
      M_AXI_ACP_MOSI   => acp_mosi,
      RS232_RTS        => RS232_RTS,
      RS232_TXD        => RS232_TXD,
      RS485_DE         => RS485_DE,
      RS485_RE_n       => RS485_RE,
      RS485_TXD        => RS485_TXD,
      SPI0_MISO        => SPI0_MISO,
      adc_enb          => adc_enb,
      dac_ebuf_rst     => dac_ebuf_rst,
      led_1            => leds(1 DOWNTO 0),
      led_2            => leds(3 DOWNTO 2),
      m_axi_hp0_tready => m_axi_hp0_tready,
      ocb_rdata_core0  => ocb_rdata_core0,
      ocb_rdata_core1  => ocb_rdata_core1,
      radio_ctrl       => radio_ctrl,
      s_axi_stream_hp0 => s_axi_stream_hp0,
      spi_config       => spi_config,
      spi_ui           => spi_ui
    );
  AXI2OCB : AXI_OCB_bridge
    PORT MAP (
      ACLK          => clksys,
      ARESETN       => ARESETN(0),
      S_AXI_AWPROT  => AXI_OCB_Core0_awprot,
      S_AXI_AWVALID => AXI_OCB_Core0_awvalid,
      S_AXI_AWADDR  => AXI_OCB_Core0_awaddr,
      S_AXI_AWREADY => AXI_OCB_Core0_awready,
      S_AXI_WVALID  => AXI_OCB_Core0_wvalid,
      S_AXI_WSTRB   => AXI_OCB_Core0_wstrb,
      S_AXI_WDATA   => AXI_OCB_Core0_wdata,
      S_AXI_WREADY  => AXI_OCB_Core0_wready,
      S_AXI_BREADY  => AXI_OCB_Core0_bready,
      S_AXI_BRESP   => AXI_OCB_Core0_bresp,
      S_AXI_BVALID  => AXI_OCB_Core0_bvalid,
      S_AXI_ARPROT  => AXI_OCB_Core0_arprot,
      S_AXI_ARVALID => AXI_OCB_Core0_arvalid,
      S_AXI_ARADDR  => AXI_OCB_Core0_araddr,
      S_AXI_ARREADY => AXI_OCB_Core0_arready,
      S_AXI_RREADY  => AXI_OCB_Core0_rready,
      S_AXI_RVALID  => AXI_OCB_Core0_rvalid,
      S_AXI_RRESP   => AXI_OCB_Core0_rresp,
      S_AXI_RDATA   => AXI_OCB_Core0_rdata,
      ocb_rdata     => ocb_rdata_core0,
      oc_bus        => oc_bus_core0
    );
  AXI2OCB1 : AXI_OCB_bridge
    PORT MAP (
      ACLK          => clksys,
      ARESETN       => ARESETN(0),
      S_AXI_AWPROT  => AXI_OCB_Core1_awprot,
      S_AXI_AWVALID => AXI_OCB_Core1_awvalid,
      S_AXI_AWADDR  => AXI_OCB_Core1_awaddr,
      S_AXI_AWREADY => AXI_OCB_Core1_awready,
      S_AXI_WVALID  => AXI_OCB_Core1_wvalid,
      S_AXI_WSTRB   => AXI_OCB_Core1_wstrb,
      S_AXI_WDATA   => AXI_OCB_Core1_wdata,
      S_AXI_WREADY  => AXI_OCB_Core1_wready,
      S_AXI_BREADY  => AXI_OCB_Core1_bready,
      S_AXI_BRESP   => AXI_OCB_Core1_bresp,
      S_AXI_BVALID  => AXI_OCB_Core1_bvalid,
      S_AXI_ARPROT  => AXI_OCB_Core1_arprot,
      S_AXI_ARVALID => AXI_OCB_Core1_arvalid,
      S_AXI_ARADDR  => AXI_OCB_Core1_araddr,
      S_AXI_ARREADY => AXI_OCB_Core1_arready,
      S_AXI_RREADY  => AXI_OCB_Core1_rready,
      S_AXI_RVALID  => AXI_OCB_Core1_rvalid,
      S_AXI_RRESP   => AXI_OCB_Core1_rresp,
      S_AXI_RDATA   => AXI_OCB_Core1_rdata,
      ocb_rdata     => ocb_rdata_core1,
      oc_bus        => oc_bus_core1
    );
  -- All LVDS pairs are swapped (N<->P) on PCB!
  LVDS_ADC : adc_lvds_if
    GENERIC MAP (
      fco_swapped_g  => '1',
      data_swapped_g => "11111111"
    )
    PORT MAP (
      LVDS_ADC_A_D0_N   => LVDS_ADC_A_D0_P,
      LVDS_ADC_A_D0_P   => LVDS_ADC_A_D0_N,
      LVDS_ADC_A_D1_N   => LVDS_ADC_A_D1_P,
      LVDS_ADC_A_D1_P   => LVDS_ADC_A_D1_N,
      LVDS_ADC_B_D0_N   => LVDS_ADC_B_D0_P,
      LVDS_ADC_B_D0_P   => LVDS_ADC_B_D0_N,
      LVDS_ADC_B_D1_N   => LVDS_ADC_B_D1_P,
      LVDS_ADC_B_D1_P   => LVDS_ADC_B_D1_N,
      LVDS_ADC_C_D0_N   => LVDS_ADC_C_D0_P,
      LVDS_ADC_C_D0_P   => LVDS_ADC_C_D0_N,
      LVDS_ADC_C_D1_N   => LVDS_ADC_C_D1_P,
      LVDS_ADC_C_D1_P   => LVDS_ADC_C_D1_N,
      LVDS_ADC_DCO_N    => LVDS_ADC_DCO_P,
      LVDS_ADC_DCO_P    => LVDS_ADC_DCO_N,
      LVDS_ADC_D_D0_N   => LVDS_ADC_D_D0_P,
      LVDS_ADC_D_D0_P   => LVDS_ADC_D_D0_N,
      LVDS_ADC_D_D1_N   => LVDS_ADC_D_D1_P,
      LVDS_ADC_D_D1_P   => LVDS_ADC_D_D1_N,
      LVDS_ADC_FCO_N    => LVDS_ADC_FCO_P,
      LVDS_ADC_FCO_P    => LVDS_ADC_FCO_N,
      adc_enb           => adc_enb,
      clk_ioctrl_200MHz => clk_idelayctrl,
      clksys            => clksys,
      sreset            => sreset(0),
      M_AXIS_ACD_A      => AXIS_ADC_A,
      M_AXIS_ACD_B      => AXIS_ADC_B,
      M_AXIS_ACD_C      => AXIS_ADC_C,
      M_AXIS_ACD_D      => AXIS_ADC_D,
      adc_status        => adc_status
    );
  Radio_DAC : radio_dac_if
    PORT MAP (
      DCLKIO     => DAC_RADIO_TX_CLK1,
      S_AXIS_DAC => AXIS_DAC,
      clksys     => clksys,
      ebuf_rst   => dac_ebuf_rst,
      DB         => DAC_RADIO_TX_D,
      ebuf_stat  => dac_ebuf_stat
    );
  iPS : wdl_ps_wrapper
    PORT MAP (
      ARESETN            => ARESETN,
      CLK_RMII_REF       => CLK_RMII_REF,
      Core0_nFIQ         => Core1_nFIQ,
      Core1_nFIQ         => Core0_nFIQ,
      DDR_addr           => DDR_addr,
      DDR_ba             => DDR_ba,
      DDR_cas_n          => DDR_cas_n,
      DDR_ck_n           => DDR_ck_n,
      DDR_ck_p           => DDR_ck_p,
      DDR_cke            => DDR_cke,
      DDR_cs_n           => DDR_cs_n,
      DDR_dm             => DDR_dm,
      DDR_dq             => DDR_dq,
      DDR_dqs_n          => DDR_dqs_n,
      DDR_dqs_p          => DDR_dqs_p,
      DDR_odt            => DDR_odt,
      DDR_ras_n          => DDR_ras_n,
      DDR_reset_n        => DDR_reset_n,
      DDR_we_n           => DDR_we_n,
      FIXED_IO_ddr_vrn   => FIXED_IO_ddr_vrn,
      FIXED_IO_ddr_vrp   => FIXED_IO_ddr_vrp,
      FIXED_IO_mio       => FIXED_IO_mio,
      FIXED_IO_ps_clk    => FIXED_IO_ps_clk,
      FIXED_IO_ps_porb   => FIXED_IO_ps_porb,
      FIXED_IO_ps_srstb  => FIXED_IO_ps_srstb,
      MDIO_mdc           => MDIO_CLK,
      M_AXIS_HP0_tdata   => M_AXIS_HP0_tdata,
      M_AXIS_HP0_tid     => M_AXIS_HP0_tid,
      M_AXIS_HP0_tkeep   => M_AXIS_HP0_tkeep,
      M_AXIS_HP0_tlast   => M_AXIS_HP0_tlast,
      M_AXIS_HP0_tready  => m_axi_hp0_tready,
      M_AXIS_HP0_tvalid  => M_AXIS_HP0_tvalid,
      M_AXI_OCB0_araddr  => AXI_OCB_Core0_araddr,
      M_AXI_OCB0_arprot  => AXI_OCB_Core0_arprot,
      M_AXI_OCB0_arready => AXI_OCB_Core0_arready,
      M_AXI_OCB0_arvalid => AXI_OCB_Core0_arvalid,
      M_AXI_OCB0_awaddr  => AXI_OCB_Core0_awaddr,
      M_AXI_OCB0_awprot  => AXI_OCB_Core0_awprot,
      M_AXI_OCB0_awready => AXI_OCB_Core0_awready,
      M_AXI_OCB0_awvalid => AXI_OCB_Core0_awvalid,
      M_AXI_OCB0_bready  => AXI_OCB_Core0_bready,
      M_AXI_OCB0_bresp   => AXI_OCB_Core0_bresp,
      M_AXI_OCB0_bvalid  => AXI_OCB_Core0_bvalid,
      M_AXI_OCB0_rdata   => AXI_OCB_Core0_rdata,
      M_AXI_OCB0_rready  => AXI_OCB_Core0_rready,
      M_AXI_OCB0_rresp   => AXI_OCB_Core0_rresp,
      M_AXI_OCB0_rvalid  => AXI_OCB_Core0_rvalid,
      M_AXI_OCB0_wdata   => AXI_OCB_Core0_wdata,
      M_AXI_OCB0_wready  => AXI_OCB_Core0_wready,
      M_AXI_OCB0_wstrb   => AXI_OCB_Core0_wstrb,
      M_AXI_OCB0_wvalid  => AXI_OCB_Core0_wvalid,
      M_AXI_OCB1_araddr  => AXI_OCB_Core1_araddr,
      M_AXI_OCB1_arprot  => AXI_OCB_Core1_arprot,
      M_AXI_OCB1_arready => AXI_OCB_Core1_arready,
      M_AXI_OCB1_arvalid => AXI_OCB_Core1_arvalid,
      M_AXI_OCB1_awaddr  => AXI_OCB_Core1_awaddr,
      M_AXI_OCB1_awprot  => AXI_OCB_Core1_awprot,
      M_AXI_OCB1_awready => AXI_OCB_Core1_awready,
      M_AXI_OCB1_awvalid => AXI_OCB_Core1_awvalid,
      M_AXI_OCB1_bready  => AXI_OCB_Core1_bready,
      M_AXI_OCB1_bresp   => AXI_OCB_Core1_bresp,
      M_AXI_OCB1_bvalid  => AXI_OCB_Core1_bvalid,
      M_AXI_OCB1_rdata   => AXI_OCB_Core1_rdata,
      M_AXI_OCB1_rready  => AXI_OCB_Core1_rready,
      M_AXI_OCB1_rresp   => AXI_OCB_Core1_rresp,
      M_AXI_OCB1_rvalid  => AXI_OCB_Core1_rvalid,
      M_AXI_OCB1_wdata   => AXI_OCB_Core1_wdata,
      M_AXI_OCB1_wready  => AXI_OCB_Core1_wready,
      M_AXI_OCB1_wstrb   => AXI_OCB_Core1_wstrb,
      M_AXI_OCB1_wvalid  => AXI_OCB_Core1_wvalid,
      PHY_IRQ_0          => IRQ_PHY1,
      PHY_IRQ_1          => IRQ_PHY2,
      RADIO_GPI          => RADIO_GPO,
      RADIO_GPIO_T       => OPEN,
      RADIO_GPO          => RADIO_GPO,
      RMII0_crs_dv       => RMII1_CRS_DV,
      RMII0_rx_er        => RMII1_RX_ER,
      RMII0_rxd          => RMII1_RX_D,
      RMII0_tx_en        => RMII1_TX_EN,
      RMII0_txd          => RMII1_TX_D,
      RMII1_crs_dv       => RMII2_CRS_DV,
      RMII1_rx_er        => RMII2_RX_ER,
      RMII1_rxd          => RMII2_RX_D,
      RMII1_tx_en        => RMII2_TX_EN,
      RMII1_txd          => RMII2_TX_D,
      SPI0_MISO          => SPI0_MISO,
      SPI0_MOSI          => SPI0_MOSI,
      SPI0_SCLK          => SPI0_SCLK,
      SPI0_SS0           => SPI0_SS(0),
      SPI0_SS1           => SPI0_SS(1),
      SPI0_SS2           => SPI0_SS(2),
      SPI_RADIO_CLK_L    => SPI_RADIO_CLK_L,
      SPI_RADIO_MISO     => SPI_RADIO_MISO,
      SPI_RADIO_MOSI     => SPI_RADIO_MOSI,
      SPI_RADIO_SEL0     => SPI_RADIO_SEL0,
      SPI_RADIO_SEL1     => SPI_RADIO_SEL1,
      SPI_RADIO_SEL2     => SPI_RADIO_SEL2,
      S_AXIS_HP0_tdata   => S_AXIS_HP0_tdata,
      S_AXIS_HP0_tid     => S_AXIS_HP0_tid,
      S_AXIS_HP0_tkeep   => S_AXIS_HP0_tkeep,
      S_AXIS_HP0_tlast   => S_AXIS_HP0_tlast,
      S_AXIS_HP0_tready  => s_axi_hp0_tready,
      S_AXIS_HP0_tvalid  => S_AXIS_HP0_tvalid,
      S_AXI_ACP_araddr   => S_AXI_ACP_araddr,
      S_AXI_ACP_arburst  => S_AXI_ACP_arburst,
      S_AXI_ACP_arcache  => S_AXI_ACP_arcache,
      S_AXI_ACP_arid     => S_AXI_ACP_arid,
      S_AXI_ACP_arlen    => S_AXI_ACP_arlen,
      S_AXI_ACP_arlock   => S_AXI_ACP_arlock,
      S_AXI_ACP_arprot   => S_AXI_ACP_arprot,
      S_AXI_ACP_arqos    => S_AXI_ACP_arqos,
      S_AXI_ACP_arready  => S_AXI_ACP_arready,
      S_AXI_ACP_arsize   => S_AXI_ACP_arsize,
      S_AXI_ACP_aruser   => S_AXI_ACP_aruser,
      S_AXI_ACP_arvalid  => S_AXI_ACP_arvalid,
      S_AXI_ACP_awaddr   => S_AXI_ACP_awaddr,
      S_AXI_ACP_awburst  => S_AXI_ACP_awburst,
      S_AXI_ACP_awcache  => S_AXI_ACP_awcache,
      S_AXI_ACP_awid     => S_AXI_ACP_awid,
      S_AXI_ACP_awlen    => S_AXI_ACP_awlen,
      S_AXI_ACP_awlock   => S_AXI_ACP_awlock,
      S_AXI_ACP_awprot   => S_AXI_ACP_awprot,
      S_AXI_ACP_awqos    => S_AXI_ACP_awqos,
      S_AXI_ACP_awready  => S_AXI_ACP_awready,
      S_AXI_ACP_awsize   => S_AXI_ACP_awsize,
      S_AXI_ACP_awuser   => S_AXI_ACP_awuser,
      S_AXI_ACP_awvalid  => S_AXI_ACP_awvalid,
      S_AXI_ACP_bid      => S_AXI_ACP_bid,
      S_AXI_ACP_bready   => S_AXI_ACP_bready,
      S_AXI_ACP_bresp    => S_AXI_ACP_bresp,
      S_AXI_ACP_bvalid   => S_AXI_ACP_bvalid,
      S_AXI_ACP_rdata    => S_AXI_ACP_rdata,
      S_AXI_ACP_rid      => S_AXI_ACP_rid,
      S_AXI_ACP_rlast    => S_AXI_ACP_rlast,
      S_AXI_ACP_rready   => S_AXI_ACP_rready,
      S_AXI_ACP_rresp    => S_AXI_ACP_rresp,
      S_AXI_ACP_rvalid   => S_AXI_ACP_rvalid,
      S_AXI_ACP_wdata    => S_AXI_ACP_wdata,
      S_AXI_ACP_wid      => S_AXI_ACP_wid,
      S_AXI_ACP_wlast    => S_AXI_ACP_wlast,
      S_AXI_ACP_wready   => S_AXI_ACP_wready,
      S_AXI_ACP_wstrb    => S_AXI_ACP_wstrb,
      S_AXI_ACP_wvalid   => S_AXI_ACP_wvalid,
      clk_idelayctrl     => clk_idelayctrl,
      clksys             => clksys,
      mdio_mdio_io       => MDIO_DAT,
      sreset             => sreset
    );
  Config_SPI : wdl_spi_cfg_if
    PORT MAP (
      spi_config           => spi_config,
      SPI_ADC_DAC_CS_ADC_n => SPI_ADC_DAC_CS_ADC,
      SPI_ADC_DAC_CS_DAC_n => SPI_ADC_DAC_CS_DAC,
      SPI_ADC_DAC_SCLK     => SPI_ADC_DAC_CLK_L,
      spi_cfg_sdi          => spi_cfg_sdi,
      SPI_ADC_DAC_MIO      => SPI_ADC_DAC_MIO
    );
  UI_SPI : wdl_spi_ui_if
    PORT MAP (
      SPI_UI_MISO     => SPI_MISO,
      spi_ui          => spi_ui,
      SPI_UI_CS_n     => SPI_CS_UI,
      SPI_UI_MOSI     => SPI_MOSI,
      SPI_UI_SCLK     => SPI_CLK_L,
      spi_ui_miso_loc => spi_ui_miso_loc
    );

END ARCHITECTURE struct;
