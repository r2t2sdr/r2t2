------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2012 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.wdl_spi_cfg_if.symbol
--
-- GENERATED:
--  Tool: Mentor Graphics HDL Designer(TM) 2012.1 (Build 6)
--  Date: 09:35:46 22.01.2015 
--
------------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.NUMERIC_STD.all;
LIBRARY wdl_lib;
USE wdl_lib.wdl_def.all;

ENTITY wdl_spi_cfg_if IS
  PORT( 
    spi_config           : IN     spi_cfg_t;
    SPI_ADC_DAC_CS_ADC_n : OUT    std_logic;
    SPI_ADC_DAC_CS_DAC_n : OUT    std_logic;
    SPI_ADC_DAC_SCLK     : OUT    std_logic;
    spi_cfg_sdi          : OUT    std_logic;
    SPI_ADC_DAC_MIO      : INOUT  std_logic
  );

-- Declarations

END ENTITY wdl_spi_cfg_if ;
------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2012 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.wdl_spi_cfg_if.struct
--
-- GENERATED:
--  Tool: Mentor Graphics HDL Designer(TM) 2012.1 (Build 6)
--  Date: 09:35:46 22.01.2015 
--
------------------------------------------------------------------------------------
LIBRARY ieee;
  USE ieee.std_logic_1164.all;
  USE ieee.NUMERIC_STD.all;
LIBRARY unisim;
  USE unisim.VCOMPONENTS.all;
LIBRARY wdl_lib;
  USE wdl_lib.wdl_def.all;


ARCHITECTURE struct OF wdl_spi_cfg_if IS

  -- Architecture declarations

  -- Internal signal declarations
  SIGNAL const_1   : std_logic;
  SIGNAL sclk      : std_logic;
  SIGNAL sdo       : std_logic;
  SIGNAL sdo_highZ : std_logic;
  SIGNAL spi_en_n  : std_logic;


  -- Component Declarations
  COMPONENT BUFT
  PORT (
    I : IN     std_ulogic;
    T : IN     std_ulogic;
    O : OUT    std_ulogic
  );
  END COMPONENT BUFT;
  COMPONENT LDPE
  GENERIC (
    INIT : bit := '1'
  );
  PORT (
    D   : IN     std_ulogic;
    G   : IN     std_ulogic;
    GE  : IN     std_ulogic;
    PRE : IN     std_ulogic;
    Q   : OUT    std_ulogic
  );
  END COMPONENT LDPE;

  -- Optional embedded configurations
  -- synthesis translate_off
  FOR ALL : BUFT USE ENTITY unisim.BUFT;
  FOR ALL : LDPE USE ENTITY unisim.LDPE;
  -- synthesis translate_on


BEGIN
  -- Architecture concurrent statements
  -- HDL Embedded Text Block 1 spi_cfg_map
  -- spi_cfg_map
  spi_en_n <= spi_config.dac_cs_n AND spi_config.adc_cs_n;
  SPI_ADC_DAC_CS_DAC_n <= spi_config.dac_cs_n;
  SPI_ADC_DAC_CS_ADC_n <= spi_config.adc_cs_n;
  sclk <= spi_config.sclk;
  sdo <= spi_config.sdo;
  sdo_highZ <= spi_config.sdo_highZ;


  -- ModuleWare code(v1.9) for instance 'const_H' of 'vdd'
  const_1 <= '1';

  -- Instance port mappings.
  SDO_BUFT : BUFT
    PORT MAP (
      O => SPI_ADC_DAC_MIO,
      I => sdo,
      T => sdo_highZ
    );
  SCLK_latch : LDPE
    GENERIC MAP (
      INIT => '1'
    )
    PORT MAP (
      Q   => SPI_ADC_DAC_SCLK,
      D   => sclk,
      G   => const_1,
      GE  => const_1,
      PRE => spi_en_n
    );
  SDI_latch : LDPE
    GENERIC MAP (
      INIT => '1'
    )
    PORT MAP (
      Q   => spi_cfg_sdi,
      D   => SPI_ADC_DAC_MIO,
      G   => const_1,
      GE  => const_1,
      PRE => spi_en_n
    );

END ARCHITECTURE struct;
