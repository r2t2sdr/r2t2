# Definitional proc to organize widgets for parameters.
proc init_gui { IPINST } {
  #Adding Page
  set Page_0 [ipgui::add_page $IPINST -name "Page 0"]
  ipgui::add_param $IPINST -name "Component_Name" -parent ${Page_0}
  ipgui::add_static_text $IPINST -name "About" -parent ${Page_0} -text {This IP Core is a simple I2C glue logic wrapper that helps connecting 
the PS7 I2C EMIO to unidirectionsl pins of the System Controller on TE0720 SoM.}


}


