# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "PCORE_CLK0_DIV" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PCORE_CLK1_DIV" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PCORE_CLKIN_PERIOD" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PCORE_DEVICE_TYPE" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PCORE_ID" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PCORE_VCO_DIV" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PCORE_VCO_MUL" -parent ${Page_0}


}

proc update_PARAM_VALUE.PCORE_CLK0_DIV { PARAM_VALUE.PCORE_CLK0_DIV } {
	# Procedure called to update PCORE_CLK0_DIV when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_CLK0_DIV { PARAM_VALUE.PCORE_CLK0_DIV } {
	# Procedure called to validate PCORE_CLK0_DIV
	return true
}

proc update_PARAM_VALUE.PCORE_CLK1_DIV { PARAM_VALUE.PCORE_CLK1_DIV } {
	# Procedure called to update PCORE_CLK1_DIV when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_CLK1_DIV { PARAM_VALUE.PCORE_CLK1_DIV } {
	# Procedure called to validate PCORE_CLK1_DIV
	return true
}

proc update_PARAM_VALUE.PCORE_CLKIN_PERIOD { PARAM_VALUE.PCORE_CLKIN_PERIOD } {
	# Procedure called to update PCORE_CLKIN_PERIOD when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_CLKIN_PERIOD { PARAM_VALUE.PCORE_CLKIN_PERIOD } {
	# Procedure called to validate PCORE_CLKIN_PERIOD
	return true
}

proc update_PARAM_VALUE.PCORE_DEVICE_TYPE { PARAM_VALUE.PCORE_DEVICE_TYPE } {
	# Procedure called to update PCORE_DEVICE_TYPE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_DEVICE_TYPE { PARAM_VALUE.PCORE_DEVICE_TYPE } {
	# Procedure called to validate PCORE_DEVICE_TYPE
	return true
}

proc update_PARAM_VALUE.PCORE_ID { PARAM_VALUE.PCORE_ID } {
	# Procedure called to update PCORE_ID when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_ID { PARAM_VALUE.PCORE_ID } {
	# Procedure called to validate PCORE_ID
	return true
}

proc update_PARAM_VALUE.PCORE_VCO_DIV { PARAM_VALUE.PCORE_VCO_DIV } {
	# Procedure called to update PCORE_VCO_DIV when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_VCO_DIV { PARAM_VALUE.PCORE_VCO_DIV } {
	# Procedure called to validate PCORE_VCO_DIV
	return true
}

proc update_PARAM_VALUE.PCORE_VCO_MUL { PARAM_VALUE.PCORE_VCO_MUL } {
	# Procedure called to update PCORE_VCO_MUL when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_VCO_MUL { PARAM_VALUE.PCORE_VCO_MUL } {
	# Procedure called to validate PCORE_VCO_MUL
	return true
}


proc update_MODELPARAM_VALUE.PCORE_ID { MODELPARAM_VALUE.PCORE_ID PARAM_VALUE.PCORE_ID } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_ID}] ${MODELPARAM_VALUE.PCORE_ID}
}

proc update_MODELPARAM_VALUE.PCORE_DEVICE_TYPE { MODELPARAM_VALUE.PCORE_DEVICE_TYPE PARAM_VALUE.PCORE_DEVICE_TYPE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_DEVICE_TYPE}] ${MODELPARAM_VALUE.PCORE_DEVICE_TYPE}
}

proc update_MODELPARAM_VALUE.PCORE_CLKIN_PERIOD { MODELPARAM_VALUE.PCORE_CLKIN_PERIOD PARAM_VALUE.PCORE_CLKIN_PERIOD } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_CLKIN_PERIOD}] ${MODELPARAM_VALUE.PCORE_CLKIN_PERIOD}
}

proc update_MODELPARAM_VALUE.PCORE_VCO_DIV { MODELPARAM_VALUE.PCORE_VCO_DIV PARAM_VALUE.PCORE_VCO_DIV } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_VCO_DIV}] ${MODELPARAM_VALUE.PCORE_VCO_DIV}
}

proc update_MODELPARAM_VALUE.PCORE_VCO_MUL { MODELPARAM_VALUE.PCORE_VCO_MUL PARAM_VALUE.PCORE_VCO_MUL } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_VCO_MUL}] ${MODELPARAM_VALUE.PCORE_VCO_MUL}
}

proc update_MODELPARAM_VALUE.PCORE_CLK0_DIV { MODELPARAM_VALUE.PCORE_CLK0_DIV PARAM_VALUE.PCORE_CLK0_DIV } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_CLK0_DIV}] ${MODELPARAM_VALUE.PCORE_CLK0_DIV}
}

proc update_MODELPARAM_VALUE.PCORE_CLK1_DIV { MODELPARAM_VALUE.PCORE_CLK1_DIV PARAM_VALUE.PCORE_CLK1_DIV } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_CLK1_DIV}] ${MODELPARAM_VALUE.PCORE_CLK1_DIV}
}

