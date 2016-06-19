add_library(r2t2 STATIC
	FreeRTOS_asm_vectors.S
	FreeRTOS_tick_config.c
	control.cpp
	main.c
	remoteproc.c
	utils.c
	r2t2.c
	r2t2_test.c
)

