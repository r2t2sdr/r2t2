------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2015 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.adc_dco_alignment_ctrl.rtl
--
-- ABSTRACT:
--  Automatic delay adjustment for the ADC data clock (DCO). 
--
--  The IDLAYCTRL uses a reference clock of 200MHz which provides a average tap 
--  delay of 78ps. => max delay = 32*78ps = 2.496ns
--  The ADC configuration:
--    FCO (sample rate): 75MHz. 
--    DCO              : 4xFCO = 300MHz => tDCO = 3.3ns
--    Operation Mode: 16-bit DDR, Two-Lane, 1 x Frame, BYTEWISE
--  Since the DCO periode is greater than the maximum adjustable delay, the phase 
--  adjustment can only be done to either of the two edge. Therefore the data 
--  channels needs to be capabel to handle the differen clock edges.  
--
-- HISTORY:
--  Date      Author      Description
--  20140826  mw          created
--
------------------------------------------------------------------------------------

LIBRARY ieee;
  USE ieee.std_logic_1164.all;
  USE ieee.NUMERIC_STD.all;
LIBRARY unisim;
  USE unisim.VCOMPONENTS.all;
LIBRARY wdl_lib;
  USE wdl_lib.wdl_def.all;


ENTITY adc_dco_alignment_ctrl IS
  PORT( 
    adc_clk_div       : IN     std_logic;
    clk_ioctrl_200MHz : IN     std_logic;
    dco_enb           : IN     std_logic;
    dco_serdes        : IN     std_logic_vector (7 DOWNTO 0);
    dly_tap_cnt       : IN     std_logic_vector (4 DOWNTO 0);
    idelayctrl_rdy    : IN     std_logic;
    clock_edge        : OUT    std_logic;
    dco_adjust_events : OUT    event_tog_vec (3 DOWNTO 0);
    dco_is_aligned    : OUT    std_logic;
    dly_ce            : OUT    std_logic;
    dly_cnt           : OUT    std_logic_vector (4 DOWNTO 0);
    dly_inc           : OUT    std_logic;
    dly_ld            : OUT    std_logic;
    dly_rst           : OUT    std_logic;
    idelay_ctrl_rst   : OUT    std_logic;
    jitter            : OUT    std_logic_vector (4 DOWNTO 0);
    serdes_ce         : OUT    std_logic;
    serdes_rst        : OUT    std_logic
  );

-- Declarations

END ENTITY adc_dco_alignment_ctrl ;

--
ARCHITECTURE rtl OF adc_dco_alignment_ctrl IS
  -- IDELAYCTRL signals...
  SIGNAL dly_rst_cnt      : unsigned(4 DOWNTO 0) := (OTHERS => '1');
  SIGNAL sync_enb         : std_logic_vector(2 DOWNTO 0)  := (OTHERS => '1');
  SIGNAL sync_rdy         : std_logic_vector(2 DOWNTO 0)  := (OTHERS => '0');
  --  
  SIGNAL adjust_enb       : std_logic := '0';
  SIGNAL alignment_done   : std_logic := '0';
  SIGNAL prescal          : unsigned(2 DOWNTO 0) := (OTHERS => '0');
  SIGNAL adjust_ce        : std_logic := '0';
  SIGNAL dco_status       : std_logic_vector(1 DOWNTO 0); -- concurrent
  SIGNAL last_dco_status  : std_logic_vector(1 DOWNTO 0) := (OTHERS => '0');
  SIGNAL skip_check       : std_logic := '1';
  SIGNAL re_init_dly      : std_logic := '0';
  --
  SUBTYPE adjust_state_t IS std_logic_vector(3 DOWNTO 0);
  CONSTANT idle_c         : adjust_state_t := "0000";
  CONSTANT first_c        : adjust_state_t := "0001";
  CONSTANT out_of_xover_c : adjust_state_t := "0010";
  CONSTANT xover_dec_c    : adjust_state_t := "0011";
  CONSTANT stable_dec_c   : adjust_state_t := "0100";
  CONSTANT stable_inc_c   : adjust_state_t := "0101";
  CONSTANT xover_inc_c    : adjust_state_t := "0110";
  CONSTANT center_xover_c : adjust_state_t := "0111";
  CONSTANT done_c         : adjust_state_t := "1000";
  CONSTANT err_c          : adjust_state_t := "1111";
  SIGNAL state : adjust_state_t := idle_c;
  -- 
  SIGNAL xover_start    : unsigned(4 DOWNTO 0) := to_unsigned(16,5);
  SIGNAL ajusted_edge   : std_logic;  -- 0 => falling, 1 => rising
  SIGNAL center_cnt     : unsigned(4 DOWNTO 0);
  SIGNAL min_warn       : event_tog := '0';
  SIGNAL max_warn       : event_tog := '0';
  SIGNAL no_edge_err    : event_tog := '0';
  SIGNAL retry          : event_tog := '0';
  -- synthesis translate_off
  SIGNAL sim_state : string(1 TO 14);
  -- synthesis translate_on

BEGIN

-- concurrent ----------------------------------------------------------------------
  dco_status(0) <= '1' WHEN dco_serdes = x"FF" ELSE '0';
  dco_status(1) <= '1' WHEN dco_serdes = x"FF" OR dco_serdes = x"00" ELSE '0';
  dco_is_aligned <= alignment_done;
  dco_adjust_events <= retry & no_edge_err & max_warn & min_warn;
-- combinational process -----------------------------------------------------------
-- clocked process -----------------------------------------------------------------
  -- IDELAYCTRL reset controller...
  dly_ctrl_rst : PROCESS(clk_ioctrl_200MHz)
  BEGIN
    IF clk_ioctrl_200MHz'EVENT AND clk_ioctrl_200MHz = '1' THEN
      -- synchronize control input to 200MHz clock...
      sync_enb <= sync_enb(1 DOWNTO 0) & dco_enb;
      sync_rdy <= sync_rdy(1 DOWNTO 0) & idelayctrl_rdy;
      IF sync_enb(2 DOWNTO 1) = "10" OR sync_rdy(2 DOWNTO 1) = "10" THEN
        dly_rst_cnt <= (OTHERS => '1'); -- load reset counter...
        idelay_ctrl_rst <= '1';
      ELSIF dly_rst_cnt(4) = '1' THEN
        dly_rst_cnt <= dly_rst_cnt - 1;
        idelay_ctrl_rst <= '1';
      ELSIF sync_enb(2) = '0' THEN
        idelay_ctrl_rst <= '1';
      ELSE
        idelay_ctrl_rst <= '0';
      END IF;      
    END IF;
  END PROCESS;
  -- 
  -- DCO delay control reset logic...
  PROCESS(idelayctrl_rdy,adc_clk_div)
  BEGIN
    IF idelayctrl_rdy = '0' THEN
      dly_rst <= '1';
      dly_ld  <= '1';
      adjust_enb <= '0';
    ELSIF adc_clk_div'EVENT AND adc_clk_div = '1' THEN
      dly_rst <= '0';
      dly_ld  <= re_init_dly;
      adjust_enb <= '1';
    END IF;
  END PROCESS;
  --
  -- DCO delay adjustment...
  PROCESS(adc_clk_div)
  BEGIN
    IF adc_clk_div'EVENT AND adc_clk_div = '1' THEN
      --
      -- clock enable each 8th cycle, for check the new 8 DCO samples...
      adjust_ce <= '0';
      IF adjust_enb = '0' OR alignment_done = '1' THEN
        prescal <= (OTHERS => '0');
      ELSE
        prescal <= prescal + 1;
      END IF;
      IF prescal = 6 THEN
        adjust_ce <= '1';
      END IF;
      --
      -- delay control logic...
      serdes_rst <= NOT adjust_enb;
      serdes_ce <= adjust_enb AND NOT alignment_done;
      dly_ce <= '0';  -- default
      re_init_dly <= '0'; -- default
      -- state machine is set to idle if the adjustment is disabled regardless 
      -- of the current state...
      IF adjust_enb = '0' THEN
        dly_cnt <= (OTHERS => '0');
        state <= idle_c;
        alignment_done <= '0';
      ELSIF adjust_ce = '1' THEN
        last_dco_status <= dco_status;
        CASE state IS
          WHEN idle_c => 
            dly_cnt <= (OTHERS => '0');
            state <= first_c;
            jitter <= (OTHERS => '0');
            alignment_done <= '0';
            retry <= NOT retry;
          --
          -- DCO status valid for the first time after adjustment has been enabled..
          WHEN first_c => 
            dly_inc <= '0';
            dly_ce <= '1';
            IF dco_status(1) = '0' THEN
              state <= out_of_xover_c;  -- search start of crossover area
            ELSE
              state <= stable_dec_c;    -- search crossover area by decreasing delay
            END IF;
          --
          -- decrease delay until the sampling point is out of the crossover area..
          WHEN out_of_xover_c =>  
            dly_inc <= '0';
            dly_ce <= '1';
            xover_start <= unsigned(dly_tap_cnt);
            IF dco_status(1) = '1' OR           -- out of crossover area..
               unsigned(dly_tap_cnt) = 0 THEN   -- .. or minimum delay reached
              state <= xover_inc_c;
              dly_inc <= '1';
            END IF;
          --
          -- search for crossover area by decreasing delay...
          WHEN stable_dec_c =>
            dly_inc <= '0'; -- state default decrease delay
            xover_start <= unsigned(dly_tap_cnt);
            ajusted_edge <= last_dco_status(0);
            IF dco_status(1) = '0' THEN -- start of crossover area
              IF unsigned(dly_tap_cnt) = 0 THEN -- minimum delay reached
                min_warn <= NOT min_warn;
                state <= done_c;
              ELSE
                state <= xover_dec_c;
                dly_ce <= '1';
              END IF;
            ELSIF (dco_status(0) XOR last_dco_status(0)) = '1' THEN
              state <= done_c;
            ELSIF unsigned(dly_tap_cnt) = 0 THEN
              state <= stable_inc_c;  -- search further by increasing the delay
              dly_inc <= '1';
              dly_ce <= '1';
            ELSE
              dly_ce <= '1';
            END IF;
          --
          -- search for end of crossover area by decreasing the delay...
          WHEN xover_dec_c =>
            dly_inc <= '0';
            center_cnt <= xover_start - unsigned(dly_tap_cnt);
            IF dco_status(0) = '1' OR unsigned(dly_tap_cnt) = 0 THEN
              jitter <= std_logic_vector(center_cnt + 1);
              state <= center_xover_c;
              dly_inc <= '1';
            END IF;
          --
          -- search for crossover area by decreasing delay...
          WHEN stable_inc_c =>
            dly_inc <= '1'; -- state default decrease delay
            xover_start <= unsigned(dly_tap_cnt);
            ajusted_edge <= NOT last_dco_status(0);
            IF dco_status(1) = '0' THEN -- start of crossover area
              IF unsigned(dly_tap_cnt) = 31 THEN -- maximum delay reached
                max_warn <= NOT max_warn;
                state <= done_c;
              ELSE
                state <= xover_inc_c;
                dly_ce <= '1';
              END IF;
            ELSIF (dco_status(0) XOR last_dco_status(0)) = '1' THEN
              state <= done_c;
            ELSIF unsigned(dly_tap_cnt) = 31 THEN
              max_warn <= NOT max_warn;
              state <= err_c;
            ELSE
              dly_ce <= '1';
            END IF;
          --
          -- search for end of crossover area by increasing the delay...
          WHEN xover_inc_c =>
            dly_inc <= '0';
            center_cnt <= unsigned(dly_tap_cnt) - xover_start;
            IF dco_status(0) = '1' OR unsigned(dly_tap_cnt) = 31 THEN
              jitter <= std_logic_vector(center_cnt + 1);
              state <= center_xover_c;
              dly_inc <= '0';
            END IF;
          --
          -- clock edge has been found, center delay to crossover area...
          WHEN center_xover_c =>
            center_cnt <= center_cnt - 1;
            IF SHIFT_RIGHT(center_cnt,1) = 0 THEN
              state <= done_c;
            ELSE
              dly_ce <= '1';
            END IF;
          --
          -- clock adjustment done...
          WHEN done_c => 
            clock_edge <= ajusted_edge;
            dly_cnt <= dly_tap_cnt;
            alignment_done <= '1';
          --
          -- assume error no edge was found, set error flag and restart...
          WHEN OTHERS =>   
            no_edge_err <= NOT no_edge_err;
            re_init_dly <= '1';
            state <= idle_c;
        END CASE;
      END IF;
    END IF; -- rising edge clksys
  END PROCESS;
-- Simulation only -----------------------------------------------------------
  -- synthesis translate_off
            WITH state SELECT
  sim_state <= "idle_c        " WHEN idle_c        ,
               "first_c       " WHEN first_c       ,
               "out_of_xover_c" WHEN out_of_xover_c,
               "xover_dec_c   " WHEN xover_dec_c   ,
               "stable_dec_c  " WHEN stable_dec_c  ,
               "stable_inc_c  " WHEN stable_inc_c  ,
               "xover_inc_c   " WHEN xover_inc_c   ,
               "center_xover_c" WHEN center_xover_c,
               "done_c        " WHEN done_c        ,
               "err_c         " WHEN err_c         ,
               "UNKNOWN       " WHEN OTHERS;
  -- synthesis translate_on
  
END ARCHITECTURE rtl;
