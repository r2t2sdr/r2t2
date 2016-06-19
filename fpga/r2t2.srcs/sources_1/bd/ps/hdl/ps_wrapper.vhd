--Copyright 1986-2015 Xilinx, Inc. All Rights Reserved.
----------------------------------------------------------------------------------
--Tool Version: Vivado v.2015.2 (lin64) Build 1266856 Fri Jun 26 16:35:25 MDT 2015
--Date        : Sat May 21 20:57:24 2016
--Host        : thinkpad running 64-bit Arch Linux
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
    gpio_tri_io : inout STD_LOGIC_VECTOR ( 31 downto 0 );
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
    GPIO_tri_i : in STD_LOGIC_VECTOR ( 31 downto 0 );
    GPIO_tri_o : out STD_LOGIC_VECTOR ( 31 downto 0 );
    GPIO_tri_t : out STD_LOGIC_VECTOR ( 31 downto 0 );
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
  signal gpio_tri_i_12 : STD_LOGIC_VECTOR ( 12 to 12 );
  signal gpio_tri_i_13 : STD_LOGIC_VECTOR ( 13 to 13 );
  signal gpio_tri_i_14 : STD_LOGIC_VECTOR ( 14 to 14 );
  signal gpio_tri_i_15 : STD_LOGIC_VECTOR ( 15 to 15 );
  signal gpio_tri_i_16 : STD_LOGIC_VECTOR ( 16 to 16 );
  signal gpio_tri_i_17 : STD_LOGIC_VECTOR ( 17 to 17 );
  signal gpio_tri_i_18 : STD_LOGIC_VECTOR ( 18 to 18 );
  signal gpio_tri_i_19 : STD_LOGIC_VECTOR ( 19 to 19 );
  signal gpio_tri_i_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal gpio_tri_i_20 : STD_LOGIC_VECTOR ( 20 to 20 );
  signal gpio_tri_i_21 : STD_LOGIC_VECTOR ( 21 to 21 );
  signal gpio_tri_i_22 : STD_LOGIC_VECTOR ( 22 to 22 );
  signal gpio_tri_i_23 : STD_LOGIC_VECTOR ( 23 to 23 );
  signal gpio_tri_i_24 : STD_LOGIC_VECTOR ( 24 to 24 );
  signal gpio_tri_i_25 : STD_LOGIC_VECTOR ( 25 to 25 );
  signal gpio_tri_i_26 : STD_LOGIC_VECTOR ( 26 to 26 );
  signal gpio_tri_i_27 : STD_LOGIC_VECTOR ( 27 to 27 );
  signal gpio_tri_i_28 : STD_LOGIC_VECTOR ( 28 to 28 );
  signal gpio_tri_i_29 : STD_LOGIC_VECTOR ( 29 to 29 );
  signal gpio_tri_i_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal gpio_tri_i_30 : STD_LOGIC_VECTOR ( 30 to 30 );
  signal gpio_tri_i_31 : STD_LOGIC_VECTOR ( 31 to 31 );
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
  signal gpio_tri_io_12 : STD_LOGIC_VECTOR ( 12 to 12 );
  signal gpio_tri_io_13 : STD_LOGIC_VECTOR ( 13 to 13 );
  signal gpio_tri_io_14 : STD_LOGIC_VECTOR ( 14 to 14 );
  signal gpio_tri_io_15 : STD_LOGIC_VECTOR ( 15 to 15 );
  signal gpio_tri_io_16 : STD_LOGIC_VECTOR ( 16 to 16 );
  signal gpio_tri_io_17 : STD_LOGIC_VECTOR ( 17 to 17 );
  signal gpio_tri_io_18 : STD_LOGIC_VECTOR ( 18 to 18 );
  signal gpio_tri_io_19 : STD_LOGIC_VECTOR ( 19 to 19 );
  signal gpio_tri_io_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal gpio_tri_io_20 : STD_LOGIC_VECTOR ( 20 to 20 );
  signal gpio_tri_io_21 : STD_LOGIC_VECTOR ( 21 to 21 );
  signal gpio_tri_io_22 : STD_LOGIC_VECTOR ( 22 to 22 );
  signal gpio_tri_io_23 : STD_LOGIC_VECTOR ( 23 to 23 );
  signal gpio_tri_io_24 : STD_LOGIC_VECTOR ( 24 to 24 );
  signal gpio_tri_io_25 : STD_LOGIC_VECTOR ( 25 to 25 );
  signal gpio_tri_io_26 : STD_LOGIC_VECTOR ( 26 to 26 );
  signal gpio_tri_io_27 : STD_LOGIC_VECTOR ( 27 to 27 );
  signal gpio_tri_io_28 : STD_LOGIC_VECTOR ( 28 to 28 );
  signal gpio_tri_io_29 : STD_LOGIC_VECTOR ( 29 to 29 );
  signal gpio_tri_io_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal gpio_tri_io_30 : STD_LOGIC_VECTOR ( 30 to 30 );
  signal gpio_tri_io_31 : STD_LOGIC_VECTOR ( 31 to 31 );
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
  signal gpio_tri_o_12 : STD_LOGIC_VECTOR ( 12 to 12 );
  signal gpio_tri_o_13 : STD_LOGIC_VECTOR ( 13 to 13 );
  signal gpio_tri_o_14 : STD_LOGIC_VECTOR ( 14 to 14 );
  signal gpio_tri_o_15 : STD_LOGIC_VECTOR ( 15 to 15 );
  signal gpio_tri_o_16 : STD_LOGIC_VECTOR ( 16 to 16 );
  signal gpio_tri_o_17 : STD_LOGIC_VECTOR ( 17 to 17 );
  signal gpio_tri_o_18 : STD_LOGIC_VECTOR ( 18 to 18 );
  signal gpio_tri_o_19 : STD_LOGIC_VECTOR ( 19 to 19 );
  signal gpio_tri_o_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal gpio_tri_o_20 : STD_LOGIC_VECTOR ( 20 to 20 );
  signal gpio_tri_o_21 : STD_LOGIC_VECTOR ( 21 to 21 );
  signal gpio_tri_o_22 : STD_LOGIC_VECTOR ( 22 to 22 );
  signal gpio_tri_o_23 : STD_LOGIC_VECTOR ( 23 to 23 );
  signal gpio_tri_o_24 : STD_LOGIC_VECTOR ( 24 to 24 );
  signal gpio_tri_o_25 : STD_LOGIC_VECTOR ( 25 to 25 );
  signal gpio_tri_o_26 : STD_LOGIC_VECTOR ( 26 to 26 );
  signal gpio_tri_o_27 : STD_LOGIC_VECTOR ( 27 to 27 );
  signal gpio_tri_o_28 : STD_LOGIC_VECTOR ( 28 to 28 );
  signal gpio_tri_o_29 : STD_LOGIC_VECTOR ( 29 to 29 );
  signal gpio_tri_o_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal gpio_tri_o_30 : STD_LOGIC_VECTOR ( 30 to 30 );
  signal gpio_tri_o_31 : STD_LOGIC_VECTOR ( 31 to 31 );
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
  signal gpio_tri_t_12 : STD_LOGIC_VECTOR ( 12 to 12 );
  signal gpio_tri_t_13 : STD_LOGIC_VECTOR ( 13 to 13 );
  signal gpio_tri_t_14 : STD_LOGIC_VECTOR ( 14 to 14 );
  signal gpio_tri_t_15 : STD_LOGIC_VECTOR ( 15 to 15 );
  signal gpio_tri_t_16 : STD_LOGIC_VECTOR ( 16 to 16 );
  signal gpio_tri_t_17 : STD_LOGIC_VECTOR ( 17 to 17 );
  signal gpio_tri_t_18 : STD_LOGIC_VECTOR ( 18 to 18 );
  signal gpio_tri_t_19 : STD_LOGIC_VECTOR ( 19 to 19 );
  signal gpio_tri_t_2 : STD_LOGIC_VECTOR ( 2 to 2 );
  signal gpio_tri_t_20 : STD_LOGIC_VECTOR ( 20 to 20 );
  signal gpio_tri_t_21 : STD_LOGIC_VECTOR ( 21 to 21 );
  signal gpio_tri_t_22 : STD_LOGIC_VECTOR ( 22 to 22 );
  signal gpio_tri_t_23 : STD_LOGIC_VECTOR ( 23 to 23 );
  signal gpio_tri_t_24 : STD_LOGIC_VECTOR ( 24 to 24 );
  signal gpio_tri_t_25 : STD_LOGIC_VECTOR ( 25 to 25 );
  signal gpio_tri_t_26 : STD_LOGIC_VECTOR ( 26 to 26 );
  signal gpio_tri_t_27 : STD_LOGIC_VECTOR ( 27 to 27 );
  signal gpio_tri_t_28 : STD_LOGIC_VECTOR ( 28 to 28 );
  signal gpio_tri_t_29 : STD_LOGIC_VECTOR ( 29 to 29 );
  signal gpio_tri_t_3 : STD_LOGIC_VECTOR ( 3 to 3 );
  signal gpio_tri_t_30 : STD_LOGIC_VECTOR ( 30 to 30 );
  signal gpio_tri_t_31 : STD_LOGIC_VECTOR ( 31 to 31 );
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
gpio_tri_iobuf_12: component IOBUF
     port map (
      I => gpio_tri_o_12(12),
      IO => gpio_tri_io(12),
      O => gpio_tri_i_12(12),
      T => gpio_tri_t_12(12)
    );
gpio_tri_iobuf_13: component IOBUF
     port map (
      I => gpio_tri_o_13(13),
      IO => gpio_tri_io(13),
      O => gpio_tri_i_13(13),
      T => gpio_tri_t_13(13)
    );
gpio_tri_iobuf_14: component IOBUF
     port map (
      I => gpio_tri_o_14(14),
      IO => gpio_tri_io(14),
      O => gpio_tri_i_14(14),
      T => gpio_tri_t_14(14)
    );
gpio_tri_iobuf_15: component IOBUF
     port map (
      I => gpio_tri_o_15(15),
      IO => gpio_tri_io(15),
      O => gpio_tri_i_15(15),
      T => gpio_tri_t_15(15)
    );
gpio_tri_iobuf_16: component IOBUF
     port map (
      I => gpio_tri_o_16(16),
      IO => gpio_tri_io(16),
      O => gpio_tri_i_16(16),
      T => gpio_tri_t_16(16)
    );
gpio_tri_iobuf_17: component IOBUF
     port map (
      I => gpio_tri_o_17(17),
      IO => gpio_tri_io(17),
      O => gpio_tri_i_17(17),
      T => gpio_tri_t_17(17)
    );
gpio_tri_iobuf_18: component IOBUF
     port map (
      I => gpio_tri_o_18(18),
      IO => gpio_tri_io(18),
      O => gpio_tri_i_18(18),
      T => gpio_tri_t_18(18)
    );
gpio_tri_iobuf_19: component IOBUF
     port map (
      I => gpio_tri_o_19(19),
      IO => gpio_tri_io(19),
      O => gpio_tri_i_19(19),
      T => gpio_tri_t_19(19)
    );
gpio_tri_iobuf_2: component IOBUF
     port map (
      I => gpio_tri_o_2(2),
      IO => gpio_tri_io(2),
      O => gpio_tri_i_2(2),
      T => gpio_tri_t_2(2)
    );
gpio_tri_iobuf_20: component IOBUF
     port map (
      I => gpio_tri_o_20(20),
      IO => gpio_tri_io(20),
      O => gpio_tri_i_20(20),
      T => gpio_tri_t_20(20)
    );
gpio_tri_iobuf_21: component IOBUF
     port map (
      I => gpio_tri_o_21(21),
      IO => gpio_tri_io(21),
      O => gpio_tri_i_21(21),
      T => gpio_tri_t_21(21)
    );
gpio_tri_iobuf_22: component IOBUF
     port map (
      I => gpio_tri_o_22(22),
      IO => gpio_tri_io(22),
      O => gpio_tri_i_22(22),
      T => gpio_tri_t_22(22)
    );
gpio_tri_iobuf_23: component IOBUF
     port map (
      I => gpio_tri_o_23(23),
      IO => gpio_tri_io(23),
      O => gpio_tri_i_23(23),
      T => gpio_tri_t_23(23)
    );
gpio_tri_iobuf_24: component IOBUF
     port map (
      I => gpio_tri_o_24(24),
      IO => gpio_tri_io(24),
      O => gpio_tri_i_24(24),
      T => gpio_tri_t_24(24)
    );
gpio_tri_iobuf_25: component IOBUF
     port map (
      I => gpio_tri_o_25(25),
      IO => gpio_tri_io(25),
      O => gpio_tri_i_25(25),
      T => gpio_tri_t_25(25)
    );
gpio_tri_iobuf_26: component IOBUF
     port map (
      I => gpio_tri_o_26(26),
      IO => gpio_tri_io(26),
      O => gpio_tri_i_26(26),
      T => gpio_tri_t_26(26)
    );
gpio_tri_iobuf_27: component IOBUF
     port map (
      I => gpio_tri_o_27(27),
      IO => gpio_tri_io(27),
      O => gpio_tri_i_27(27),
      T => gpio_tri_t_27(27)
    );
gpio_tri_iobuf_28: component IOBUF
     port map (
      I => gpio_tri_o_28(28),
      IO => gpio_tri_io(28),
      O => gpio_tri_i_28(28),
      T => gpio_tri_t_28(28)
    );
gpio_tri_iobuf_29: component IOBUF
     port map (
      I => gpio_tri_o_29(29),
      IO => gpio_tri_io(29),
      O => gpio_tri_i_29(29),
      T => gpio_tri_t_29(29)
    );
gpio_tri_iobuf_3: component IOBUF
     port map (
      I => gpio_tri_o_3(3),
      IO => gpio_tri_io(3),
      O => gpio_tri_i_3(3),
      T => gpio_tri_t_3(3)
    );
gpio_tri_iobuf_30: component IOBUF
     port map (
      I => gpio_tri_o_30(30),
      IO => gpio_tri_io(30),
      O => gpio_tri_i_30(30),
      T => gpio_tri_t_30(30)
    );
gpio_tri_iobuf_31: component IOBUF
     port map (
      I => gpio_tri_o_31(31),
      IO => gpio_tri_io(31),
      O => gpio_tri_i_31(31),
      T => gpio_tri_t_31(31)
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
      GPIO_tri_i(31) => gpio_tri_i_31(31),
      GPIO_tri_i(30) => gpio_tri_i_30(30),
      GPIO_tri_i(29) => gpio_tri_i_29(29),
      GPIO_tri_i(28) => gpio_tri_i_28(28),
      GPIO_tri_i(27) => gpio_tri_i_27(27),
      GPIO_tri_i(26) => gpio_tri_i_26(26),
      GPIO_tri_i(25) => gpio_tri_i_25(25),
      GPIO_tri_i(24) => gpio_tri_i_24(24),
      GPIO_tri_i(23) => gpio_tri_i_23(23),
      GPIO_tri_i(22) => gpio_tri_i_22(22),
      GPIO_tri_i(21) => gpio_tri_i_21(21),
      GPIO_tri_i(20) => gpio_tri_i_20(20),
      GPIO_tri_i(19) => gpio_tri_i_19(19),
      GPIO_tri_i(18) => gpio_tri_i_18(18),
      GPIO_tri_i(17) => gpio_tri_i_17(17),
      GPIO_tri_i(16) => gpio_tri_i_16(16),
      GPIO_tri_i(15) => gpio_tri_i_15(15),
      GPIO_tri_i(14) => gpio_tri_i_14(14),
      GPIO_tri_i(13) => gpio_tri_i_13(13),
      GPIO_tri_i(12) => gpio_tri_i_12(12),
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
      GPIO_tri_o(31) => gpio_tri_o_31(31),
      GPIO_tri_o(30) => gpio_tri_o_30(30),
      GPIO_tri_o(29) => gpio_tri_o_29(29),
      GPIO_tri_o(28) => gpio_tri_o_28(28),
      GPIO_tri_o(27) => gpio_tri_o_27(27),
      GPIO_tri_o(26) => gpio_tri_o_26(26),
      GPIO_tri_o(25) => gpio_tri_o_25(25),
      GPIO_tri_o(24) => gpio_tri_o_24(24),
      GPIO_tri_o(23) => gpio_tri_o_23(23),
      GPIO_tri_o(22) => gpio_tri_o_22(22),
      GPIO_tri_o(21) => gpio_tri_o_21(21),
      GPIO_tri_o(20) => gpio_tri_o_20(20),
      GPIO_tri_o(19) => gpio_tri_o_19(19),
      GPIO_tri_o(18) => gpio_tri_o_18(18),
      GPIO_tri_o(17) => gpio_tri_o_17(17),
      GPIO_tri_o(16) => gpio_tri_o_16(16),
      GPIO_tri_o(15) => gpio_tri_o_15(15),
      GPIO_tri_o(14) => gpio_tri_o_14(14),
      GPIO_tri_o(13) => gpio_tri_o_13(13),
      GPIO_tri_o(12) => gpio_tri_o_12(12),
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
      GPIO_tri_t(31) => gpio_tri_t_31(31),
      GPIO_tri_t(30) => gpio_tri_t_30(30),
      GPIO_tri_t(29) => gpio_tri_t_29(29),
      GPIO_tri_t(28) => gpio_tri_t_28(28),
      GPIO_tri_t(27) => gpio_tri_t_27(27),
      GPIO_tri_t(26) => gpio_tri_t_26(26),
      GPIO_tri_t(25) => gpio_tri_t_25(25),
      GPIO_tri_t(24) => gpio_tri_t_24(24),
      GPIO_tri_t(23) => gpio_tri_t_23(23),
      GPIO_tri_t(22) => gpio_tri_t_22(22),
      GPIO_tri_t(21) => gpio_tri_t_21(21),
      GPIO_tri_t(20) => gpio_tri_t_20(20),
      GPIO_tri_t(19) => gpio_tri_t_19(19),
      GPIO_tri_t(18) => gpio_tri_t_18(18),
      GPIO_tri_t(17) => gpio_tri_t_17(17),
      GPIO_tri_t(16) => gpio_tri_t_16(16),
      GPIO_tri_t(15) => gpio_tri_t_15(15),
      GPIO_tri_t(14) => gpio_tri_t_14(14),
      GPIO_tri_t(13) => gpio_tri_t_13(13),
      GPIO_tri_t(12) => gpio_tri_t_12(12),
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
