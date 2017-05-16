set project_name r2t2

open_project ../project/$project_name.xpr

reset_project
make_wrapper -files [get_files ../bd/ps.bd] -top
generate_target all [get_files  ../bd/ps/ps.bd]

reset_run synth_1
if {[get_property PROGRESS [get_runs synth_1]] != "100%"} {
 launch_runs synth_1 
 wait_on_run synth_1
}

if {[get_property PROGRESS [get_runs impl_1]] != "100%"} {
  reset_run impl_1
  launch_runs impl_1 -to_step route_design
  wait_on_run impl_1
}

open_run [get_runs impl_1]

write_bitstream -force -file $project_name.bit
write_hwdef -force  -file $project_name.hdf

close_project
