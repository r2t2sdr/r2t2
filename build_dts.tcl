open_hw_design top.hdf
set_repo_path device-tree-xlnx
create_sw_design device-tree -os device_tree -proc ps7_cortexa9_0
generate_target -dir tree
quit

# in system.dts muss noch ethernet.dtsi included werden, sonst wird der phy nicht erkannt
