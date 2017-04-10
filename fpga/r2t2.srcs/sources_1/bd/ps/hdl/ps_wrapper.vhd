--Copyright 1986-2016 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2016.2 (lin64) Build 1577090 Thu Jun  2 16:32:35 MDT 2016
--Date        : Mon Aug  8 17:06:28 2016
--Host        : beta running 64-bit unknown
--Command     : generate_target ps_wrapper.bd
--Design      : ps_wrapper
--Purpose     : IP block netlist
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity ps_wrapper is
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
    clk_12mhz : out STD_LOGIC_VECTOR ( 0 to 0 );
    clk_idelayctrl : out STD_LOGIC;
    gpio_tri_io : inout STD_LOGIC_VECTOR ( 11 downto 0 );
    hdmi_out_clk : out STD_LOGIC;
    hdmi_out_data : out STD_LOGIC_VECTOR ( 11 downto 0 );
    hdmi_out_de : out STD_LOGIC;
    hdmi_out_hsync : out STD_LOGIC;
    hdmi_out_vsync : out STD_LOGIC;
    i2s_mdk : out STD_LOGIC;
    iic_0_scl_io : inout STD_LOGIC;
    iic_0_sda_io : inout STD_LOGIC;
    pl_clk : in STD_LOGIC;
    sys_clk : out STD_LOGIC
  );
end ps_wrapper;

architecture STRUCTURE of ps_wrapper is
  component ps is
  port (
    DDR_cas_n : inout STD_LOGIC;
    DDR_cke : inout STD_LOGIC;
    DDR_ck_n : inout STD_LOGIC;
    DDR_ck_p : inout STD_LOGIC;
    DDR_cs_n : inout STD_LOGIC;
    DDR_reset_n : inout STD_LOGIC;
    DDR_odt : inout STD_LOGIC;
    DDR_ras_n : inout STD_LOGIC;
    DDR_we_n : inout STD_LOGIC;
    DDR_ba : inout STD_LOGIC_VECTOR ( 2 downto 0 );
    DDR_addr : inout STD_LOGIC_VECTOR ( 14 downto 0 );
    DDR_dm : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    DDR_dq : inout STD_LOGIC_VECTOR ( 31 downto 0 );
    DDR_dqs_n : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    DDR_dqs_p : inout STD_LOGIC_VECTOR ( 3 downto 0 );
    FIXED_IO_mio : inout STD_LOGIC_VECTOR ( 53 downto 0 );
    FIXED_IO_ddr_vrn : inout STD_LOGIC;
    FIXED_IO_ddr_vrp : inout STD_LOGIC;
    FIXED_IO_ps_srstb : inout STD_LOGIC;
    FIXED_IO_ps_clk : inout STD_LOGIC;
    FIXED_IO_ps_porb : inout STD_LOGIC;
    IIC_0_sda_i : in STD_LOGIC;
    IIC_0_sda_o : out STD_LOGIC;
    IIC_0_sda_t : out STD_LOGIC;
    IIC_0_scl_i : in STD_LOGIC;
    IIC_0_scl_o : out STD_LOGIC;
    IIC_0_scl_t : out STD_LOGIC;
    GPIO_tri_i : in STD_LOGIC_VECTOR ( 11 downto 0 );
    GPIO_tri_o : out STD_LOGIC_VECTOR ( 11 downto 0 );
    GPIO_tri_t : out STD_LOGIC_VECTOR ( 11 downto 0 );
    I2S_bclk : out STD_LOGIC_VECTOR ( 0 to 0 );
    I2S_lrclk : out STD_LOGIC_VECTOR ( 0 to 0 );
    I2S_sdata_out : out STD_LOGIC_VECTOR ( 0 to 0 );
    I2S_sdata_in : in STD_LOGIC_VECTOR ( 0 to 0 );
    hdmi_out_de : out STD_LOGIC;
    hdmi_out_clk : out STD_LOGIC;
    hdmi_out_vsync : out STD_LOGIC;
    hdmi_out_data : out STD_LOGIC_VECTOR ( 11 downto 0 );
    hdmi_out_hsync : out STD_LOGIC;
    sys_clk : out STD_LOGIC;
    PL_PIN_K16 : in STD_LOGIC;
    PL_PIN_K19 : in STD_LOGIC;
    PL_PIN_M15 : in STD_LOGIC;
    PL_PIN_N15 : in STD_LOGIC;
    PL_PIN_P16 : in STD_LOGIC;
    PL_PIN_P22 : in STD_LOGIC;
    PL_PIN_L16 : out STD_LOGIC;
    PL_PIN_K20 : out STD_LOGIC;
    PL_PIN_N22 : out STD_LOGIC;
    PHY_LED0 : out STD_LOGIC;
    PHY_LED1 : out STD_LOGIC;
    PHY_LED2 : out STD_LOGIC;
    clk_idelayctrl : out STD_LOGIC;
    pl_clk : in STD_LOGIC;
    LVDS_ADC_A_D0_N : in STD_LOGIC;
    LVDS_ADC_A_D0_P : in STD_LOGIC;
    LVDS_ADC_A_D1_N : in STD_LOGIC;
    LVDS_ADC_A_D1_P : in STD_LOGIC;
    LVDS_ADC_B_D0_P : in STD_LOGIC;
    LVDS_ADC_B_D0_N : in STD_LOGIC;
    LVDS_ADC_B_D1_N : in STD_LOGIC;
    LVDS_ADC_B_D1_P : in STD_LOGIC;
    LVDS_ADC_DCO_N : in STD_LOGIC;
    LVDS_ADC_DCO_P : in STD_LOGIC;
    LVDS_ADC_FCO_N : in STD_LOGIC;
    LVDS_ADC_FCO_P : in STD_LOGIC;
    DCLKIO : in STD_LOGIC;
    DB : out STD_LOGIC_VECTOR ( 13 downto 0 );
    i2s_mdk : out STD_LOGIC;
    clk_12mhz : out STD_LOGIC_VECTOR ( 0 to 0 )
  );
  end component ps;
  component IOBUF is
  port (
    I : in STD_LOGIC;
    O : out STD_LOGIC;
    T : in STD_LOGIC;
    IO : inout STD_LOGIC
  );
  end component IOBUF;
  signal gpio_tri_i_0 : STD_LOGIC_VECTOR ( 0 to 0 );
  signal gpio_tri_i_1 : STD_LOGIC_VECTOR ( 1 to 1 );
  signal gpio_tri_i_10 : STD_LOGIC_VECTOR ( 10 to 10 );
  signal gpio_tri_i_11 : STD_LOGIC_VECTOR ( 11 to 11 );
  signal gpio_tri_i_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal gpio_tri_i_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal gpio_tri_i_4 : STD_LOGIC_VECTOR ( 4 to 4 );
  signal gpio_tri_i_5 : STD_LOGIC_VECTOR ( 5 to 5 );
  signal gpio_tri_i_6 : STD_LOGIC_VECTOR ( 6 to 6 );
  signal gpio_tri_i_7 : STD_LOGIC_VECTOR ( 7 to 7 );
  signal gpio_tri_i_8 : STD_LOGIC_VECTOR ( 8 to 8 );
  signal gpio_tri_i_9 : STD_LOGIC_VECTOR ( 9 to 9 );
  signal gpio_tri_io_0 : STD_LOGIC_VECTOR ( 0 to 0 );
  signal gpio_tri_io_1 : STD_LOGIC_VECTOR ( 1 to 1 );
  signal gpio_tri_io_10 : STD_LOGIC_VECTOR ( 10 to 10 );
  signal gpio_tri_io_11 : STD_LOGIC_VECTOR ( 11 to 11 );
  signal gpio_tri_io_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal gpio_tri_io_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal gpio_tri_io_4 : STD_LOGIC_VECTOR ( 4 to 4 );
  signal gpio_tri_io_5 : STD_LOGIC_VECTOR ( 5 to 5 );
  signal gpio_tri_io_6 : STD_LOGIC_VECTOR ( 6 to 6 );
  signal gpio_tri_io_7 : STD_LOGIC_VECTOR ( 7 to 7 );
  signal gpio_tri_io_8 : STD_LOGIC_VECTOR ( 8 to 8 );
  signal gpio_tri_io_9 : STD_LOGIC_VECTOR ( 9 to 9 );
  signal gpio_tri_o_0 : STD_LOGIC_VECTOR ( 0 to 0 );
  signal gpio_tri_o_1 : STD_LOGIC_VECTOR ( 1 to 1 );
  signal gpio_tri_o_10 : STD_LOGIC_VECTOR ( 10 to 10 );
  signal gpio_tri_o_11 : STD_LOGIC_VECTOR ( 11 to 11 );
  signal gpio_tri_o_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal gpio_tri_o_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal gpio_tri_o_4 : STD_LOGIC_VECTOR ( 4 to 4 );
  signal gpio_tri_o_5 : STD_LOGIC_VECTOR ( 5 to 5 );
  signal gpio_tri_o_6 : STD_LOGIC_VECTOR ( 6 to 6 );
  signal gpio_tri_o_7 : STD_LOGIC_VECTOR ( 7 to 7 );
  signal gpio_tri_o_8 : STD_LOGIC_VECTOR ( 8 to 8 );
  signal gpio_tri_o_9 : STD_LOGIC_VECTOR ( 9 to 9 );
  signal gpio_tri_t_0 : STD_LOGIC_VECTOR ( 0 to 0 );
  signal gpio_tri_t_1 : STD_LOGIC_VECTOR ( 1 to 1 );
  signal gpio_tri_t_10 : STD_LOGIC_VECTOR ( 10 to 10 );
  signal gpio_tri_t_11 : STD_LOGIC_VECTOR ( 11 to 11 );
  signal gpio_tri_t_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal gpio_tri_t_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal gpio_tri_t_4 : STD_LOGIC_VECTOR ( 4 to 4 );
  signal gpio_tri_t_5 : STD_LOGIC_VECTOR ( 5 to 5 );
  signal gpio_tri_t_6 : STD_LOGIC_VECTOR ( 6 to 6 );
  signal gpio_tri_t_7 : STD_LOGIC_VECTOR ( 7 to 7 );
  signal gpio_tri_t_8 : STD_LOGIC_VECTOR ( 8 to 8 );
  signal gpio_tri_t_9 : STD_LOGIC_VECTOR ( 9 to 9 );
  signal iic_0_scl_i : STD_LOGIC;
  signal iic_0_scl_o : STD_LOGIC;
  signal iic_0_scl_t : STD_LOGIC;
  signal iic_0_sda_i : STD_LOGIC;
  signal iic_0_sda_o : STD_LOGIC;
  signal iic_0_sda_t : STD_LOGIC;
begin
gpio_tri_iobuf_0: component IOBUF
     port map (
      I => gpio_tri_o_0(0),
      IO => gpio_tri_io(0),
      O => gpio_tri_i_0(0),
      T => gpio_tri_t_0(0)
    );
gpio_tri_iobuf_1: component IOBUF
     port map (
      I => gpio_tri_o_1(1),
      IO => gpio_tri_io(1),
      O => gpio_tri_i_1(1),
      T => gpio_tri_t_1(1)
    );
gpio_tri_iobuf_10: component IOBUF
     port map (
      I => gpio_tri_o_10(10),
      IO => gpio_tri_io(10),
      O => gpio_tri_i_10(10),
      T => gpio_tri_t_10(10)
    );
gpio_tri_iobuf_11: component IOBUF
     port map (
      I => gpio_tri_o_11(11),
      IO => gpio_tri_io(11),
      O => gpio_tri_i_11(11),
      T => gpio_tri_t_11(11)
    );
gpio_tri_iobuf_2: component IOBUF
     port map (
      I => gpio_tri_o_2(2),
      IO => gpio_tri_io(2),
      O => gpio_tri_i_2(2),
      T => gpio_tri_t_2(2)
    );
gpio_tri_iobuf_3: component IOBUF
     port map (
      I => gpio_tri_o_3(3),
      IO => gpio_tri_io(3),
      O => gpio_tri_i_3(3),
      T => gpio_tri_t_3(3)
    );
gpio_tri_iobuf_4: component IOBUF
     port map (
      I => gpio_tri_o_4(4),
      IO => gpio_tri_io(4),
      O => gpio_tri_i_4(4),
      T => gpio_tri_t_4(4)
    );
gpio_tri_iobuf_5: component IOBUF
     port map (
      I => gpio_tri_o_5(5),
      IO => gpio_tri_io(5),
      O => gpio_tri_i_5(5),
      T => gpio_tri_t_5(5)
    );
gpio_tri_iobuf_6: component IOBUF
     port map (
      I => gpio_tri_o_6(6),
      IO => gpio_tri_io(6),
      O => gpio_tri_i_6(6),
      T => gpio_tri_t_6(6)
    );
gpio_tri_iobuf_7: component IOBUF
     port map (
      I => gpio_tri_o_7(7),
      IO => gpio_tri_io(7),
      O => gpio_tri_i_7(7),
      T => gpio_tri_t_7(7)
    );
gpio_tri_iobuf_8: component IOBUF
     port map (
      I => gpio_tri_o_8(8),
      IO => gpio_tri_io(8),
      O => gpio_tri_i_8(8),
      T => gpio_tri_t_8(8)
    );
gpio_tri_iobuf_9: component IOBUF
     port map (
      I => gpio_tri_o_9(9),
      IO => gpio_tri_io(9),
      O => gpio_tri_i_9(9),
      T => gpio_tri_t_9(9)
    );
iic_0_scl_iobuf: component IOBUF
     port map (
      I => iic_0_scl_o,
      IO => iic_0_scl_io,
      O => iic_0_scl_i,
      T => iic_0_scl_t
    );
iic_0_sda_iobuf: component IOBUF
     port map (
      I => iic_0_sda_o,
      IO => iic_0_sda_io,
      O => iic_0_sda_i,
      T => iic_0_sda_t
    );
ps_i: component ps
     port map (
      DB(13 downto 0) => DB(13 downto 0),
      DCLKIO => DCLKIO,
      DDR_addr(14 downto 0) => DDR_addr(14 downto 0),
      DDR_ba(2 downto 0) => DDR_ba(2 downto 0),
      DDR_cas_n => DDR_cas_n,
      DDR_ck_n => DDR_ck_n,
      DDR_ck_p => DDR_ck_p,
      DDR_cke => DDR_cke,
      DDR_cs_n => DDR_cs_n,
      DDR_dm(3 downto 0) => DDR_dm(3 downto 0),
      DDR_dq(31 downto 0) => DDR_dq(31 downto 0),
      DDR_dqs_n(3 downto 0) => DDR_dqs_n(3 downto 0),
      DDR_dqs_p(3 downto 0) => DDR_dqs_p(3 downto 0),
      DDR_odt => DDR_odt,
      DDR_ras_n => DDR_ras_n,
      DDR_reset_n => DDR_reset_n,
      DDR_we_n => DDR_we_n,
      FIXED_IO_ddr_vrn => FIXED_IO_ddr_vrn,
      FIXED_IO_ddr_vrp => FIXED_IO_ddr_vrp,
      FIXED_IO_mio(53 downto 0) => FIXED_IO_mio(53 downto 0),
      FIXED_IO_ps_clk => FIXED_IO_ps_clk,
      FIXED_IO_ps_porb => FIXED_IO_ps_porb,
      FIXED_IO_ps_srstb => FIXED_IO_ps_srstb,
      GPIO_tri_i(11) => gpio_tri_i_11(11),
      GPIO_tri_i(10) => gpio_tri_i_10(10),
      GPIO_tri_i(9) => gpio_tri_i_9(9),
      GPIO_tri_i(8) => gpio_tri_i_8(8),
      GPIO_tri_i(7) => gpio_tri_i_7(7),
      GPIO_tri_i(6) => gpio_tri_i_6(6),
      GPIO_tri_i(5) => gpio_tri_i_5(5),
      GPIO_tri_i(4) => gpio_tri_i_4(4),
      GPIO_tri_i(3) => gpio_tri_i_3(3),
      GPIO_tri_i(2) => gpio_tri_i_2(2),
      GPIO_tri_i(1) => gpio_tri_i_1(1),
      GPIO_tri_i(0) => gpio_tri_i_0(0),
      GPIO_tri_o(11) => gpio_tri_o_11(11),
      GPIO_tri_o(10) => gpio_tri_o_10(10),
      GPIO_tri_o(9) => gpio_tri_o_9(9),
      GPIO_tri_o(8) => gpio_tri_o_8(8),
      GPIO_tri_o(7) => gpio_tri_o_7(7),
      GPIO_tri_o(6) => gpio_tri_o_6(6),
      GPIO_tri_o(5) => gpio_tri_o_5(5),
      GPIO_tri_o(4) => gpio_tri_o_4(4),
      GPIO_tri_o(3) => gpio_tri_o_3(3),
      GPIO_tri_o(2) => gpio_tri_o_2(2),
      GPIO_tri_o(1) => gpio_tri_o_1(1),
      GPIO_tri_o(0) => gpio_tri_o_0(0),
      GPIO_tri_t(11) => gpio_tri_t_11(11),
      GPIO_tri_t(10) => gpio_tri_t_10(10),
      GPIO_tri_t(9) => gpio_tri_t_9(9),
      GPIO_tri_t(8) => gpio_tri_t_8(8),
      GPIO_tri_t(7) => gpio_tri_t_7(7),
      GPIO_tri_t(6) => gpio_tri_t_6(6),
      GPIO_tri_t(5) => gpio_tri_t_5(5),
      GPIO_tri_t(4) => gpio_tri_t_4(4),
      GPIO_tri_t(3) => gpio_tri_t_3(3),
      GPIO_tri_t(2) => gpio_tri_t_2(2),
      GPIO_tri_t(1) => gpio_tri_t_1(1),
      GPIO_tri_t(0) => gpio_tri_t_0(0),
      I2S_bclk(0) => I2S_bclk(0),
      I2S_lrclk(0) => I2S_lrclk(0),
      I2S_sdata_in(0) => I2S_sdata_in(0),
      I2S_sdata_out(0) => I2S_sdata_out(0),
      IIC_0_scl_i => iic_0_scl_i,
      IIC_0_scl_o => iic_0_scl_o,
      IIC_0_scl_t => iic_0_scl_t,
      IIC_0_sda_i => iic_0_sda_i,
      IIC_0_sda_o => iic_0_sda_o,
      IIC_0_sda_t => iic_0_sda_t,
      LVDS_ADC_A_D0_N => LVDS_ADC_A_D0_N,
      LVDS_ADC_A_D0_P => LVDS_ADC_A_D0_P,
      LVDS_ADC_A_D1_N => LVDS_ADC_A_D1_N,
      LVDS_ADC_A_D1_P => LVDS_ADC_A_D1_P,
      LVDS_ADC_B_D0_N => LVDS_ADC_B_D0_N,
      LVDS_ADC_B_D0_P => LVDS_ADC_B_D0_P,
      LVDS_ADC_B_D1_N => LVDS_ADC_B_D1_N,
      LVDS_ADC_B_D1_P => LVDS_ADC_B_D1_P,
      LVDS_ADC_DCO_N => LVDS_ADC_DCO_N,
      LVDS_ADC_DCO_P => LVDS_ADC_DCO_P,
      LVDS_ADC_FCO_N => LVDS_ADC_FCO_N,
      LVDS_ADC_FCO_P => LVDS_ADC_FCO_P,
      PHY_LED0 => PHY_LED0,
      PHY_LED1 => PHY_LED1,
      PHY_LED2 => PHY_LED2,
      PL_PIN_K16 => PL_PIN_K16,
      PL_PIN_K19 => PL_PIN_K19,
      PL_PIN_K20 => PL_PIN_K20,
      PL_PIN_L16 => PL_PIN_L16,
      PL_PIN_M15 => PL_PIN_M15,
      PL_PIN_N15 => PL_PIN_N15,
      PL_PIN_N22 => PL_PIN_N22,
      PL_PIN_P16 => PL_PIN_P16,
      PL_PIN_P22 => PL_PIN_P22,
      clk_12mhz(0) => clk_12mhz(0),
      clk_idelayctrl => clk_idelayctrl,
      hdmi_out_clk => hdmi_out_clk,
      hdmi_out_data(11 downto 0) => hdmi_out_data(11 downto 0),
      hdmi_out_de => hdmi_out_de,
      hdmi_out_hsync => hdmi_out_hsync,
      hdmi_out_vsync => hdmi_out_vsync,
      i2s_mdk => i2s_mdk,
      pl_clk => pl_clk,
      sys_clk => sys_clk
    );
end STRUCTURE;
