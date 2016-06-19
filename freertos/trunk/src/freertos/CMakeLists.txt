add_library(freertos STATIC
	inbyte.c
	list.c
	outbyte.c
	queue.c
	tasks.c
	timers.c
	portable/MemMang/heap_3.c
	portable/GCC/ARM_CA9/port.c
	portable/GCC/ARM_CA9/portASM.S
	croutine.c  
	event_groups.c
)
