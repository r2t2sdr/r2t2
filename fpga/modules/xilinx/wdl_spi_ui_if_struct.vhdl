------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2012 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.wdl_spi_ui_if.symbol
--
-- GENERATED:
--  Tool: Mentor Graphics HDL Designer(TM) 2012.1 (Build 6)
--  Date: 09:35:54 22.01.2015 
--
------------------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.NUMERIC_STD.all;
LIBRARY wdl_lib;
USE wdl_lib.wdl_def.all;

ENTITY wdl_spi_ui_if IS
  PORT( 
    SPI_UI_MISO     : IN     std_logic;
    spi_ui          : IN     spi_ui_t;
    SPI_UI_CS_n     : OUT    std_logic;
    SPI_UI_MOSI     : OUT    std_logic;
    SPI_UI_SCLK     : OUT    std_logic;
    spi_ui_miso_loc : OUT    std_logic
  );

-- Declarations

END ENTITY wdl_spi_ui_if ;
------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2012 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.wdl_spi_ui_if.struct
--
-- GENERATED:
--  Tool: Mentor Graphics HDL Designer(TM) 2012.1 (Build 6)
--  Date: 09:35:54 22.01.2015 
--
------------------------------------------------------------------------------------
LIBRARY ieee;
  USE ieee.std_logic_1164.all;
  USE ieee.NUMERIC_STD.all;
LIBRARY unisim;
  USE unisim.VCOMPONENTS.all;
LIBRARY wdl_lib;
  USE wdl_lib.wdl_def.all;


ARCHITECTURE struct OF wdl_spi_ui_if IS

  -- Architecture declarations

  -- Internal signal declarations
  SIGNAL const_1  : std_logic;
  SIGNAL mosi     : std_logic;
  SIGNAL sclk     : std_logic;
  SIGNAL spi_en_n : std_logic;


  -- Component Declarations
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
  FOR ALL : LDPE USE ENTITY unisim.LDPE;
  -- synthesis translate_on


BEGIN
  -- Architecture concurrent statements
  -- HDL Embedded Text Block 1 spi_mapper
  -- user interface SPI mapper
  SPI_UI_CS_n <= spi_ui.cs_n;
  spi_en_n <= spi_ui.cs_n;
  sclk  <= spi_ui.sclk;
  mosi  <= spi_ui.mosi;


  -- ModuleWare code(v1.9) for instance 'const_H' of 'vdd'
  const_1 <= '1';

  -- Instance port mappings.
  MISO_latch : LDPE
    GENERIC MAP (
      INIT => '1'
    )
    PORT MAP (
      Q   => spi_ui_miso_loc,
      D   => SPI_UI_MISO,
      G   => const_1,
      GE  => const_1,
      PRE => spi_en_n
    );
  MOSI_latch : LDPE
    GENERIC MAP (
      INIT => '1'
    )
    PORT MAP (
      Q   => SPI_UI_MOSI,
      D   => mosi,
      G   => const_1,
      GE  => const_1,
      PRE => spi_en_n
    );
  SCLK_latch : LDPE
    GENERIC MAP (
      INIT => '1'
    )
    PORT MAP (
      Q   => SPI_UI_SCLK,
      D   => sclk,
      G   => const_1,
      GE  => const_1,
      PRE => spi_en_n
    );

END ARCHITECTURE struct;
