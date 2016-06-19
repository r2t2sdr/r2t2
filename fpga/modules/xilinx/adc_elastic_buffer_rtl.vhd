------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2015 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.adc_elastic_buffer.rtl
--
-- ABSTRACT:
--  Elastic buffer for the ADC interface, used for the clock domain crossing between
--  the ADC data clock DCO and the system clock.
--  The DCO rate is half of the sysclk period.
--
-- HISTORY:
--  Date      Author      Description
--  20140909  mw          created
--
------------------------------------------------------------------------------------

LIBRARY ieee;
  USE ieee.std_logic_1164.all;
  USE ieee.NUMERIC_STD.all;
LIBRARY wdl_lib;
  USE wdl_lib.wdl_def.all;

ENTITY adc_elastic_buffer IS
  PORT( 
    -- write port
    adc_clk_div    : IN     std_logic;
    fco_aligned    : IN     std_logic;
    adc_data_in    : IN     std_logic_vector (15 DOWNTO 0);
    adc_valid_in   : IN     std_logic;
    -- read port
    buf_reset      : IN     std_logic;
    clksys         : IN     std_logic;
    stream_ref     : IN     event_enb;
    status         : OUT    ebuf_status_t;
    buf_ready      : OUT    std_logic;
    adc_data_out   : OUT    std_logic_vector (15 DOWNTO 0);
    data_valid_out : OUT    std_logic
  );

-- Declarations

END ENTITY adc_elastic_buffer ;

--
ARCHITECTURE rtl OF adc_elastic_buffer IS
  -- elastic buffer distributed RAM...
  TYPE buffer_t IS ARRAY(0 TO 31) OF std_logic_vector(15 DOWNTO 0);
  SIGNAL elastic_buf  : buffer_t := (OTHERS => (OTHERS => '0'));
  SIGNAL waddr  : unsigned(4 DOWNTO 0);
  SIGNAL wdata  : std_logic_vector(15 DOWNTO 0);
  SIGNAL we     : std_logic := '0';
  SIGNAL rdata  : std_logic_vector(15 DOWNTO 0) := (OTHERS => '0');
  SIGNAL raddr  : unsigned(4 DOWNTO 0);
  --
  -- write port controller..
  SIGNAL wr_ptr : unsigned(5 DOWNTO 0)  := (OTHERS => '0');
  --
  -- read port controller..
  SIGNAL rd_ptr : unsigned(5 DOWNTO 0)  := (OTHERS => '0');
  SIGNAL wr1_sync : std_logic_vector(2 DOWNTO 0) := (OTHERS => '0');
  SIGNAL wr_level : unsigned(5 DOWNTO 0) := (OTHERS => '0');
  SIGNAL fco_aligned_meta : std_logic := '0';
  SIGNAL adc_run  : std_logic;
  SIGNAL fill_level : unsigned(5 DOWNTO 0);
  SIGNAL ebuf_status : ebuf_status_t := (OTHERS => '0');
    ALIAS buf_error     : std_logic IS ebuf_status(2);
    ALIAS gt_threshold  : std_logic IS ebuf_status(1);
    ALIAS lt_threshold  : std_logic IS ebuf_status(0);
  SIGNAL stream_enb : std_logic := '0';
  SIGNAL reset_seq : std_logic_vector(1 DOWNTO 0);
BEGIN
-- concurrent ----------------------------------------------------------------------
  raddr <= rd_ptr(raddr'RANGE);
  status <= ebuf_status;
  buf_ready <= stream_enb;
-- combinational process -----------------------------------------------------------
-- clocked process -----------------------------------------------------------------
  --
  -- elastic buffer distributed RAM...  
  write_port : PROCESS(adc_clk_div)
  BEGIN
    IF adc_clk_div'EVENT AND adc_clk_div = '1' THEN
      IF we = '1' THEN
        elastic_buf(TO_INTEGER(waddr)) <= wdata;
      END IF;
    END IF;
  END PROCESS;
  --
  read_port : PROCESS(clksys)
  BEGIN
    IF clksys'EVENT AND clksys = '1' THEN
      rdata <= elastic_buf(TO_INTEGER(raddr));
    END IF;
  END PROCESS;
  --
  -- Write control...
  write_ctrl : PROCESS(adc_clk_div)
  BEGIN
    IF adc_clk_div'EVENT AND adc_clk_div = '1' THEN
      we <= '0';  -- default
      IF fco_aligned = '1' THEN
        wdata <= adc_data_in;
        we <= adc_valid_in;
        waddr <= wr_ptr(waddr'RANGE);
        IF adc_valid_in = '1' THEN
          wr_ptr <= wr_ptr + 1;
        END IF;
      ELSE
        wr_ptr <= (OTHERS => '0');
      END IF;
    END IF;
  END PROCESS;
  --
  -- Read control...
  read_ctrl : PROCESS(clksys)
  BEGIN
    IF clksys'EVENT AND clksys = '1' THEN
      --
      -- synchronize signals from ADC clock domain...
      fco_aligned_meta <= fco_aligned;
      adc_run <= fco_aligned_meta;
      wr1_sync <= wr1_sync(1 DOWNTO 0) & wr_ptr(1);
      --
      -- buffer level observation...
      IF adc_run = '0' THEN
        fill_level <= (OTHERS => '0');
        wr_level <= (OTHERS => '0');
      ELSE
        fill_level <= wr_level - rd_ptr;
        IF (wr1_sync(2) XOR wr1_sync(1)) = '1' THEN
          wr_level <= wr_ptr(5 DOWNTO 1) & '1';
        END IF;
      END IF;
      IF buf_reset = '1' THEN 
        ebuf_status <= (OTHERS => '0');
      ELSIF stream_enb = '1' THEN
        IF fill_level(5 DOWNTO 3) = "000" THEN
          lt_threshold <= '1';
        END IF;
        IF fill_level(5 DOWNTO 3) = "111" THEN
          gt_threshold <= '1';
        END IF;
        IF fill_level = 0 THEN
          buf_error <= '1';
        ELSIF fill_level = 32 THEN
          buf_error <= '1';
        END IF;
      END IF;
      --
      -- buffer reset sequence:
      --  1. clear reset_seq on buf_reset
      --  2. wait until frame alignment is lost
      --  3. wait until buffer is half full
      IF buf_reset = '1' THEN
        reset_seq <= (OTHERS => '0');
      ELSIF adc_run = '0' THEN
        reset_seq(0) <= '1';
      ELSIF fill_level(fill_level'HIGH) = '1' THEN
        reset_seq(1) <= reset_seq(0);
      END IF;
      IF buf_reset = '1' THEN
        stream_enb <= '0';
      ELSIF reset_seq = "11" THEN
        stream_enb <= '1';
      END IF;
      --
      -- data streaming...
      data_valid_out <= '0';  -- default
      IF stream_enb = '1' THEN
        IF stream_ref = '1' THEN
          data_valid_out <= '1';
          adc_data_out <= rdata;
          rd_ptr <= rd_ptr + 1;
        END IF;
      ELSE
        rd_ptr <= (OTHERS => '0');
      END IF;
    END IF;
  END PROCESS;
END ARCHITECTURE rtl;
