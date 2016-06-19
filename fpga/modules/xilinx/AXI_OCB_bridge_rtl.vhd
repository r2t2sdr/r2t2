------------------------------------------------------------------------------------
-- (c) COPYRIGHT 2015 by NetModule AG, Switzerland.  All rights reserved.
--
-- The program(s) may only be used and/or copied with the written permission
-- from NetModule AG or in accordance with the terms and conditions stipulated
-- in the agreement contract under which the program(s) have been supplied.
--
-- VHDL ENTITY/ARCHITECTURE: wdl_xilinx.AXI_OCB_bridge.rtl
--
-- ABSTRACT:
--  This component provides the interface to connect the OCB to an AXI4 Master, using
--  the AXI4-Lite slave interface.
--  There is one OCB-interface for the LRE and another one for the management of the
--  Zynq4Ethernet platform. On the AXI side, a 128k Address range, where the lower
--  64k are reserved for the management and the upper for the LRE.
--  Contains also the version register.
--
--  AXI4 specification:
--  - Reset:
--      The AXI protocol uses a single active LOW reset signal, ARESETn. The reset 
--      signal can be asserted asynchronously, but deassertion must be synchronous
--      with a rising edge of ACLK.
--      A slave interface must drive RVALID and BVALID LOW!
--      According to this requirement, these two signals are handled in a separate
--      process with an asynchronous reset.
--
--  - Clock:
--      Each AXI component uses a single clock signal, ACLK. All input signals are 
--      sampled on the rising edge of ACLK. All output signal changes must occur 
--      after the rising edge of ACLK. On master and slave interfaces there must 
--      be no combinatorial paths between input and output signals.
--
--  - Handshake:
--      A source is not permitted to wait until READY is asserted before asserting
--      VALID. But a destination is permitted to wait for VALID to be asserted 
--      before asserting the corresponding READY.
--      Once VALID is asserted it must remain asserted until the handshake occurs, 
--      at a rising clock edge at which VALID and READY are both asserted.
--
--  - Read/Write response
--      If the decoded oc_bus index is greater than ocb_cnt_g this bridges returns 
--      an decode error (DECERR). In all other cases a OKAY is returnd.
--
--  - Access control:
--      OCB does not support simultaneous read and write cycles, therefore the 
--      following
--      priorities are used:
--        highest 1. active cycle
--                2. read cycle
--        lowest  3. write cycle
--
--  - Read cycle:
--      After a read request has been detected, it takes "ocb_read_latency_c" clock
--      cycles, until the data is provided to the AXI-Master and the RVALID is 
--      asserted. ARREADY is always asserted during IDLE state, it is deasserted in
--      all other states.
--      Up to 7 clock cycles read latency is supported.
--
--  - Write cycle:
--      AWREADY remains deasserted until AWVALID is asserted. WREADY is asserted in 
--      the same cycle as AWREADY is asserted. As a consequence of that, the write
--      data channel is stuck until the address has been presented by the master!
--
--
-- HISTORY:
--  Date      Author      Description
--  20140825  mw          created
--
------------------------------------------------------------------------------------

LIBRARY ieee;
  USE ieee.std_logic_1164.all;
  USE ieee.NUMERIC_STD.all;
LIBRARY wdl_lib;
  USE wdl_lib.wdl_def.all;

ENTITY AXI_OCB_bridge IS
  PORT( 
    ACLK          : IN     std_logic;
    ARESETN       : IN     std_logic;
    -- write address channel
    S_AXI_AWPROT  : IN     std_logic_vector (2 DOWNTO 0);
    S_AXI_AWVALID : IN     std_logic_vector (0 DOWNTO 0);
    S_AXI_AWADDR  : IN     std_logic_vector (31 DOWNTO 0);
    S_AXI_AWREADY : OUT    std_logic_vector (0 DOWNTO 0);
    -- write data channel
    S_AXI_WVALID  : IN     std_logic_vector (0 DOWNTO 0);
    S_AXI_WSTRB   : IN     std_logic_vector (3 DOWNTO 0);
    S_AXI_WDATA   : IN     std_logic_vector (31 DOWNTO 0);
    S_AXI_WREADY  : OUT    std_logic_vector (0 DOWNTO 0);
    -- write response channel
    S_AXI_BREADY  : IN     std_logic_vector (0 DOWNTO 0);
    S_AXI_BRESP   : OUT    std_logic_vector (1 DOWNTO 0);
    S_AXI_BVALID  : OUT    std_logic_vector (0 DOWNTO 0);
    -- read address channel
    S_AXI_ARPROT  : IN     std_logic_vector (2 DOWNTO 0);
    S_AXI_ARVALID : IN     std_logic_vector (0 DOWNTO 0);
    S_AXI_ARADDR  : IN     std_logic_vector (31 DOWNTO 0);
    S_AXI_ARREADY : OUT    std_logic_vector (0 DOWNTO 0);
    -- read response channel
    S_AXI_RREADY  : IN     std_logic_vector (0 DOWNTO 0);
    S_AXI_RVALID  : OUT    std_logic_vector (0 DOWNTO 0);
    S_AXI_RRESP   : OUT    std_logic_vector (1 DOWNTO 0);
    S_AXI_RDATA   : OUT    std_logic_vector (31 DOWNTO 0);
    -- OC-Bus
    ocb_rdata     : IN     ocb_data_t;
    oc_bus        : OUT    oc_bus_t
  );

-- Declarations

END ENTITY AXI_OCB_bridge ;

--
ARCHITECTURE rtl OF AXI_OCB_bridge IS
  SUBTYPE axi_resp_t IS std_logic_vector(1 DOWNTO 0);
    CONSTANT axi_okay_c   : axi_resp_t := "00";
    CONSTANT axi_exokay_c : axi_resp_t := "01";
    CONSTANT axi_slverr_c : axi_resp_t := "10";
    CONSTANT axi_decerr_c : axi_resp_t := "11";
  -- FSM
  SUBTYPE state_t IS std_logic_vector(3 DOWNTO 0);
  CONSTANT idle_c   : state_t := "0000";
  CONSTANT rcyc_c   : state_t := "0001";
  CONSTANT rdat_c   : state_t := "0011";
  CONSTANT rdone_c  : state_t := "0010";
  CONSTANT wdat_c   : state_t := "0100";
  CONSTANT wcyc_c   : state_t := "1100";
  CONSTANT wdone_c  : state_t := "1000";
  SIGNAL state : state_t := idle_c;
  -- AXI read-back 
  SIGNAL BVALID : std_logic_vector(0 DOWNTO 0) := (OTHERS => '0');
  SIGNAL RVALID : std_logic_vector(0 DOWNTO 0) := (OTHERS => '0');
  -- oc-bus
  SIGNAL addr_valid : std_logic := '0';
  SIGNAL cyc        : std_logic := '0';
  SIGNAL re         : event_enb := '0';
  SIGNAL web        : event_enb_vec(3 DOWNTO 0) := (OTHERS => '0');
  SIGNAL addr       : ocb_addr_t := (OTHERS => '0');
  SIGNAL wdata      : ocb_data_t := (OTHERS => '0');
  SIGNAL wstrb      : std_logic_vector(3 DOWNTO 0);
  SIGNAL cycle_cnt  : unsigned(2 DOWNTO 0);
  -- for local registers
  SIGNAL ocb_loc      : oc_bus_t;
  SIGNAL read_array   : ocb_reg_space_t(0 DOWNTO 0);
  SIGNAL addr_match   : std_logic_vector(0 DOWNTO 0);
  SIGNAL loc_rdata    : ocb_data_t;
  -- synthesis translate_off
  SIGNAL sim_state  : string(1 TO 7);
  -- synthesis translate_on
BEGIN
-- concurrent ----------------------------------------------------------------------
  --
  S_AXI_BVALID <= BVALID WHEN ARESETN = '1' ELSE "0";
  S_AXI_RVALID <= RVALID WHEN ARESETN = '1' ELSE "0";  
  --
  ocb_loc <= (cyc    => cyc,
              re     => re,
              web    => web,
              addr   => addr,
              wdata  => wdata);  
  oc_bus <= ocb_loc;
-- combinational process -----------------------------------------------------------
-- clocked process -----------------------------------------------------------------  
  PROCESS(ACLK)
  BEGIN
    IF ACLK'EVENT AND ACLK = '1' THEN
      -- write response handshake..
      IF BVALID = "1" AND S_AXI_BREADY = "1" THEN
        BVALID <= "0";        
      ELSIF state = wdone_c THEN
        BVALID <= "1";
      END IF;
      --
      -- read response handshake..      
      IF RVALID = "1" AND S_AXI_RREADY = "1" THEN
        RVALID <= "0";        
      ELSIF state = rdat_c OR state = rdone_c THEN
        RVALID <= "1";        
      END IF;
      --
      -- defaults ..
      S_AXI_AWREADY <= "0";
      S_AXI_WREADY  <= "0";
      S_AXI_ARREADY <= "0";
      cyc <= '0';
      re  <= '0';
      web <= (OTHERS => '0');
      cycle_cnt <= (OTHERS => '0'); -- counts only during read cycle
      --
      -- FSM:
      CASE state IS
        --
        -- IDLE
        WHEN idle_c =>
          -- always ready for read access during idle, read has highest priority..
          S_AXI_ARREADY <= "1";         
          S_AXI_BRESP <= axi_okay_c;  -- default
          S_AXI_RRESP <= axi_okay_c;  -- default
          addr_valid <= '1';  -- default
          IF S_AXI_ARVALID = "1" THEN
            IF unsigned(S_AXI_ARADDR AND x"BFFF0000") /= 0 THEN
              addr_valid <= '0';
            END IF;
            S_AXI_ARREADY <= "0";
            addr <= S_AXI_ARADDR(addr'LENGTH-1 DOWNTO 0);
            state <= rcyc_c;
          -- write request..
          ELSIF S_AXI_AWVALID = "1" THEN
            S_AXI_AWREADY <= "1";
            S_AXI_WREADY  <= "1";
            addr <= S_AXI_AWADDR(addr'LENGTH-1 DOWNTO 0);
            IF S_AXI_WVALID = "1" THEN  -- write data already valid.
              wdata <= S_AXI_WDATA;
              wstrb <= S_AXI_WSTRB;
              state <= wcyc_c;
            ELSE
              state <= wdat_c;
            END IF;
          END IF;
        -- 
        -- get write data..
        WHEN wdat_c =>
          S_AXI_WREADY <= NOT S_AXI_WVALID;
          IF S_AXI_WVALID = "1" THEN
            wdata <= S_AXI_WDATA;
            wstrb <= S_AXI_WSTRB;
            state <= wcyc_c;
          END IF;
        --
        -- write cycle..
        WHEN wcyc_c =>
          IF addr_valid = '1' THEN
            cyc <= '1';
            IF cyc = '1' THEN  -- write strobe is assigned
              web   <= wstrb;    -- ..during the second cycle
              state <= wdone_c;
            END IF;
          ELSE
            state <= wdone_c;
          END IF;
        --
        -- AXI write acknowledge..
        WHEN wdone_c =>
          cyc <= cyc; -- keep current value
          IF BVALID = "1" AND S_AXI_BREADY = "1" THEN
            state <= idle_c;
          END IF;
          IF addr_valid = '0' THEN
            S_AXI_BRESP <= axi_slverr_c;  -- address is out of range
          END IF;
        --
        -- Read cycle..
        WHEN rcyc_c =>
          cyc <= addr_valid; -- keep current value
          cycle_cnt <= cycle_cnt + 1;
          IF cycle_cnt = 1 THEN -- assign read event during the second cycle 
            re <= cyc; 
          END IF;
          IF cycle_cnt = ocb_read_latency_c THEN
            state <= rdat_c;
          END IF;
        --
        -- catch read data..
        WHEN rdat_c =>
          IF addr_valid = '0' THEN
            S_AXI_RRESP <= axi_slverr_c;  -- address is out of range
          END IF;
          S_AXI_RDATA <= ocb_rdata;-- OR loc_rdata;
          state <= rdone_c;
        --
        -- AXI read acknowledge
        WHEN rdone_c =>
          IF addr_valid = '0' THEN
            S_AXI_RRESP <= axi_slverr_c;  -- address is out of range
          END IF;
          IF RVALID = "1" AND S_AXI_RREADY = "1" THEN
            state <= idle_c;
            S_AXI_RDATA <= (OTHERS => '0');
          END IF;
        --
        -- illegal 
        WHEN OTHERS => 
          state <= idle_c;
      END CASE;
    END IF;
  END PROCESS;
  --
  -- version_reg : PROCESS(ACLK)
    -- VARIABLE addr_ind_v  : natural;
  -- BEGIN
    -- IF ACLK'EVENT AND ACLK = '1' THEN
      -- IF sreset = '1' THEN
        -- addr_match <= (OTHERS => '0');
        -- loc_rdata <= (OTHERS => '0');
      -- ELSE  
        -- addr_ind_v := 0;
        -- -- read-only registers
        -- ocb_readport_p(x"0000"&IDS_VERSION_c, ids_version_reg_c, addr_match, addr_ind_v, ocb_loc, read_array);
        -- loc_rdata <= ocb_rdata_or_f(read_array);
      -- END IF;
    -- END IF;
  -- END PROCESS;   

-- simulation only --
  -- synthesis translate_off
              WITH state SELECT
  sim_state <= "idle_c " WHEN idle_c ,
               "rcyc_c " WHEN rcyc_c ,
               "rdat_c " WHEN rdat_c ,
               "rdone_c" WHEN rdone_c,
               "wdat_c " WHEN wdat_c ,
               "wcyc_c " WHEN wcyc_c ,
               "wdone_c" WHEN wdone_c,
               "UNKNOWN" WHEN OTHERS;
  -- synthesis translate_on  
END ARCHITECTURE rtl;
