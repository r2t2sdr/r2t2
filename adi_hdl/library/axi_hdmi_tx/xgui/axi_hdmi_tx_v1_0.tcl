# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  ipgui::add_param $IPINST -name "Component_Name"
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "PCORE_Cr_Cb_N" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PCORE_DEVICE_TYPE" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PCORE_EMBEDDED_SYNC" -parent ${Page_0}
  ipgui::add_param $IPINST -name "PCORE_ID" -parent ${Page_0}


}

proc update_PARAM_VALUE.PCORE_Cr_Cb_N { PARAM_VALUE.PCORE_Cr_Cb_N } {
	# Procedure called to update PCORE_Cr_Cb_N when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_Cr_Cb_N { PARAM_VALUE.PCORE_Cr_Cb_N } {
	# Procedure called to validate PCORE_Cr_Cb_N
	return true
}

proc update_PARAM_VALUE.PCORE_DEVICE_TYPE { PARAM_VALUE.PCORE_DEVICE_TYPE } {
	# Procedure called to update PCORE_DEVICE_TYPE when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_DEVICE_TYPE { PARAM_VALUE.PCORE_DEVICE_TYPE } {
	# Procedure called to validate PCORE_DEVICE_TYPE
	return true
}

proc update_PARAM_VALUE.PCORE_EMBEDDED_SYNC { PARAM_VALUE.PCORE_EMBEDDED_SYNC } {
	# Procedure called to update PCORE_EMBEDDED_SYNC when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_EMBEDDED_SYNC { PARAM_VALUE.PCORE_EMBEDDED_SYNC } {
	# Procedure called to validate PCORE_EMBEDDED_SYNC
	return true
}

proc update_PARAM_VALUE.PCORE_ID { PARAM_VALUE.PCORE_ID } {
	# Procedure called to update PCORE_ID when any of the dependent parameters in the arguments change
}

proc validate_PARAM_VALUE.PCORE_ID { PARAM_VALUE.PCORE_ID } {
	# Procedure called to validate PCORE_ID
	return true
}


proc update_MODELPARAM_VALUE.PCORE_ID { MODELPARAM_VALUE.PCORE_ID PARAM_VALUE.PCORE_ID } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_ID}] ${MODELPARAM_VALUE.PCORE_ID}
}

proc update_MODELPARAM_VALUE.PCORE_Cr_Cb_N { MODELPARAM_VALUE.PCORE_Cr_Cb_N PARAM_VALUE.PCORE_Cr_Cb_N } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_Cr_Cb_N}] ${MODELPARAM_VALUE.PCORE_Cr_Cb_N}
}

proc update_MODELPARAM_VALUE.PCORE_DEVICE_TYPE { MODELPARAM_VALUE.PCORE_DEVICE_TYPE PARAM_VALUE.PCORE_DEVICE_TYPE } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_DEVICE_TYPE}] ${MODELPARAM_VALUE.PCORE_DEVICE_TYPE}
}

proc update_MODELPARAM_VALUE.PCORE_EMBEDDED_SYNC { MODELPARAM_VALUE.PCORE_EMBEDDED_SYNC PARAM_VALUE.PCORE_EMBEDDED_SYNC } {
	# Procedure called to set VHDL generic/Verilog parameter value(s) based on TCL parameter value
	set_property value [get_property value ${PARAM_VALUE.PCORE_EMBEDDED_SYNC}] ${MODELPARAM_VALUE.PCORE_EMBEDDED_SYNC}
}

