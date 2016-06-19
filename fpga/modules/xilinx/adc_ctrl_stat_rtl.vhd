------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2015 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.adc_ctrl_stat.rtl
--
-- ABSTRACT:
--  Control the elastic buffer for the individual channel according to the adc_enb
--  input. 
--  Synchronize the status signals from the ADC clock to the system clock domain.
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

ENTITY adc_ctrl_stat IS
  PORT( 
    adc_enb           : IN     std_logic_vector (3 DOWNTO 0);
    buf_ready         : IN     std_logic_vector (3 DOWNTO 0);
    buf_status        : IN     std_logic_vector (11 DOWNTO 0);
    clksys            : IN     std_logic;
    clock_edge        : IN     std_logic;
    dco_adjust_events : IN     event_tog_vec (3 DOWNTO 0);
    dco_is_aligned    : IN     std_logic;
    dly_cnt           : IN     std_logic_vector (4 DOWNTO 0);
    fco_bitslip_cnt   : IN     std_logic_vector (7 DOWNTO 0);
    fco_err           : IN     std_logic;
    fco_is_aligned    : IN     std_logic;
    fco_lost          : IN     std_logic;
    frame_pattern     : IN     std_logic_vector (7 DOWNTO 0);
    jitter            : IN     std_logic_vector (4 DOWNTO 0);
    sreset            : IN     std_logic;
    adc_status        : OUT    adc_status_t;
    buf_reset         : OUT    std_logic_vector (3 DOWNTO 0);
    dco_enb           : OUT    std_logic;
    stream_ref        : OUT    event_enb
  );

-- Declarations

END ENTITY adc_ctrl_stat ;

--
ARCHITECTURE rtl OF adc_ctrl_stat IS
  -- clock domain crossing...
  SIGNAL dco_aligned_sync   : std_logic_vector(2 DOWNTO 0)  := (OTHERS => '0');
  SIGNAL dco_no_edge_sync   : std_logic_vector(2 DOWNTO 0)  := (OTHERS => '0');
  SIGNAL dco_min_warn_sync  : std_logic_vector(2 DOWNTO 0)  := (OTHERS => '0');
  SIGNAL dco_max_warn_sync  : std_logic_vector(2 DOWNTO 0)  := (OTHERS => '0');
  SIGNAL fco_aligned_sync   : std_logic_vector(2 DOWNTO 0)  := (OTHERS => '0');
  SIGNAL fco_err_sync       : std_logic_vector(2 DOWNTO 0)  := (OTHERS => '0');
  SIGNAL fco_lost_sync      : std_logic_vector(2 DOWNTO 0)  := (OTHERS => '0');
  SIGNAL retry_sync         : std_logic_vector(2 DOWNTO 0)  := (OTHERS => '0');
  SIGNAL dco_align_iter_cnt : unsigned(8 DOWNTO 0)  := (OTHERS => '0');
  --
  SIGNAL stream_ref_tog : event_tog := '0'; -- 75MHz ADC stream rate
  --
  TYPE bufstats IS ARRAY(3 DOWNTO 0) OF ebuf_status_t;
  SIGNAL ebuf_stats : bufstats                      := (OTHERS => (OTHERS => '0'));
  SIGNAL ebuf_rdy   : std_logic_vector(3 DOWNTO 0)  := (OTHERS => '0');
  --
  SIGNAL channel_enab : std_logic_vector(3 DOWNTO 0);
BEGIN
-- concurrent ----------------------------------------------------------------------
  stream_ref <= stream_ref_tog;
  --
  adc_status.ch_A_ready     <= ebuf_rdy(3);
  adc_status.ch_A_buf_stat  <= ebuf_stats(3);
  adc_status.ch_B_ready     <= ebuf_rdy(2);
  adc_status.ch_B_buf_stat  <= ebuf_stats(2);
  adc_status.ch_C_ready     <= ebuf_rdy(1);
  adc_status.ch_C_buf_stat  <= ebuf_stats(1);
  adc_status.ch_D_ready     <= ebuf_rdy(0);
  adc_status.ch_D_buf_stat  <= ebuf_stats(0);
  adc_status.dco_align_iter <= STD_LOGIC_VECTOR(dco_align_iter_cnt(7 DOWNTO 0));
  --
  ch_enb_gen : FOR i IN 0 TO 3 GENERATE
    channel_enab(3-i) <= adc_enb(i);
  END GENERATE;
-- combinational process -----------------------------------------------------------
-- clocked process -----------------------------------------------------------------
  PROCESS(clksys)
  BEGIN
    IF clksys'EVENT AND clksys = '1' THEN
      -- clock domain crossing...
      dco_aligned_sync  <= dco_aligned_sync(1 DOWNTO 0)  & dco_is_aligned;
      retry_sync        <= retry_sync(1 DOWNTO 0)        & dco_adjust_events(3);
      dco_no_edge_sync  <= dco_no_edge_sync(1 DOWNTO 0)  & dco_adjust_events(2);
      dco_max_warn_sync <= dco_max_warn_sync(1 DOWNTO 0) & dco_adjust_events(1);
      dco_min_warn_sync <= dco_min_warn_sync(1 DOWNTO 0) & dco_adjust_events(0);
      fco_aligned_sync  <= fco_aligned_sync(1 DOWNTO 0)  & fco_is_aligned;
      fco_err_sync      <= fco_err_sync(1 DOWNTO 0)      & fco_err;
      fco_lost_sync     <= fco_lost_sync(1 DOWNTO 0)     & fco_lost;
      --
      IF sreset = '1' THEN
        dco_enb <= '0';
        stream_ref_tog <= '0';
        buf_reset <= (OTHERS => '1');
        dco_align_iter_cnt  <= (OTHERS => '0');
        adc_status.dco_isaligned    <= '0';
        adc_status.dco_aligned_edge <= '0';
        adc_status.dco_no_edge      <= '0';
        adc_status.dco_max_warn     <= '0';
        adc_status.dco_min_warn     <= '0';
        adc_status.dco_jitter       <= (OTHERS => '0');
        adc_status.fco_isaligned    <= '0';
        adc_status.fco_error        <= '0';
        adc_status.fco_lock_lost    <= '0';
        adc_status.fco_bitslip_cnt  <= (OTHERS => '0');
        --
      ELSE
        --
        -- DCO alignment status...
        adc_status.dco_isaligned  <= dco_aligned_sync(2);
        adc_status.dco_no_edge    <= dco_no_edge_sync(2) XOR dco_no_edge_sync(1);
        adc_status.dco_max_warn   <= dco_no_edge_sync(2) XOR dco_no_edge_sync(1);
        adc_status.dco_min_warn   <= dco_no_edge_sync(2) XOR dco_no_edge_sync(1);
        IF dco_aligned_sync(2 DOWNTO 1) = "01"  THEN  -- rising edge
          adc_status.dco_dly_tap_cnt  <= dly_cnt;
          adc_status.dco_jitter <= jitter;
        END IF;
        --
        -- FCO alignment status...
        adc_status.fco_isaligned <= fco_aligned_sync(2);
        IF dco_aligned_sync(2) = '0' THEN
          adc_status.fco_error  <= '0';
        ELSE
          IF (fco_err_sync(2) XOR fco_err_sync(1)) = '1' THEN
            adc_status.fco_error  <= '1';
          END IF;
          IF fco_aligned_sync(2 DOWNTO 1) = "10" THEN
            adc_status.fco_lock_lost <= '0';
            adc_status.fco_bitslip_cnt <= fco_bitslip_cnt;
          ELSIF (fco_lost_sync(2) XOR fco_lost_sync(1)) = '1' THEN
            adc_status.fco_lock_lost <= '1';
          END IF;
        END IF;
        --
        -- channel specific...
        IF channel_enab = "0000" THEN  -- all ADCs disabled  
          stream_ref_tog <= '0';
          dco_enb <= '0';      
        ELSE
          dco_enb <= '1';
          IF dco_aligned_sync(1) = '1' THEN 
            stream_ref_tog <= NOT stream_ref_tog;
            -- if DCO is aligned, release the reset of the buffers,
            -- whereas the channel is enable...
            FOR i IN channel_enab'RANGE LOOP
              IF channel_enab(i) = '1' THEN
                buf_reset(i)  <= '0';
                ebuf_stats(i) <= buf_status(3*i+2 DOWNTO 3*i);
                ebuf_rdy(i)   <= buf_ready(i);
              ELSE
                buf_reset(i)  <= '1';
                ebuf_stats(i) <= (OTHERS => '0');
                ebuf_rdy(i)   <= '0';
              END IF;
            END LOOP;
          END IF;
        END IF;
      END IF;
    END IF;
  END PROCESS;
END ARCHITECTURE rtl;
