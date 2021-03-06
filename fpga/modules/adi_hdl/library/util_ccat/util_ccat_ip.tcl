# ip

source ../scripts/adi_env.tcl
source $ad_hdl_dir/library/scripts/adi_ip.tcl

adi_ip_create util_ccat
adi_ip_files util_ccat [list \
  "util_ccat.v" \
  "util_ccat_constr.xdc" ]

adi_ip_properties_lite util_ccat
adi_ip_constraints util_ccat [list \
  "util_ccat_constr.xdc" ]

set_property -dict {driver_value {0} enablement_dependency {spirit:decode(id('MODELPARAM_VALUE.CH_CNT')) > 1}} \
  [ipx::get_port data_1 [ipx::current_core]] \


set_property -dict {driver_value {0} enablement_dependency {spirit:decode(id('MODELPARAM_VALUE.CH_CNT')) > 2}} \
  [ipx::get_port data_2 [ipx::current_core]] \


set_property -dict {driver_value {0} enablement_dependency {spirit:decode(id('MODELPARAM_VALUE.CH_CNT')) > 3}} \
  [ipx::get_port data_3 [ipx::current_core]] \


set_property -dict {driver_value {0} enablement_dependency {spirit:decode(id('MODELPARAM_VALUE.CH_CNT')) > 4}} \
  [ipx::get_port data_4 [ipx::current_core]] \


set_property -dict {driver_value {0} enablement_dependency {spirit:decode(id('MODELPARAM_VALUE.CH_CNT')) > 5}} \
  [ipx::get_port data_5 [ipx::current_core]] \


set_property -dict {driver_value {0} enablement_dependency {spirit:decode(id('MODELPARAM_VALUE.CH_CNT')) > 6}} \
  [ipx::get_port data_6 [ipx::current_core]] \


set_property -dict {driver_value {0} enablement_dependency {spirit:decode(id('MODELPARAM_VALUE.CH_CNT')) > 7}} \
  [ipx::get_port data_7 [ipx::current_core]] \


ipx::save_core [ipx::current_core]


