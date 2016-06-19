#include "FreeRTOS.h"
#include "task.h"

#include "r2t2.h"
#include "remoteproc.h"
#include "sleep.h"
#include "xil_printf.h"

/* FIXME: Testing */
#include "r2t2_test.h"
#include "utils.h"

//#include "rx_dsp.h"
//#include "tx_dsp.h"
#include "control.h"
#include "xuartps.h"

#include "xaxidma.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "xdebug.h"


extern XUartPs uartPtr;

/* Latency Sampler Task */
static void task_poll_fpga(void *pvParameters)
{
	while(1)
	{
		/* TODO: Read data from FPGA */
		/* TODO: Send data to linux */
		sleep(1);
		XUartPs_Send(&uartPtr, "x", 1);

	}
}


/* -------------------------------------------------------------------------- */
void message_handler(unsigned char* data, unsigned int len)
{
	/* TODO: Send data to FPGA */

	/* FIXME: Remove me for testing */
	r2t2_test_handle_skb(data, len);
}

/* -------------------------------------------------------------------------- */

void init_process() {
	initControl();
	//initRxDSP();
	//initTXDSP();
}


void r2t2_init(void)
{

	init_process();
	/* Create FPGA polling task ... */
	xTaskCreate(task_poll_fpga, "R2T2", configMINIMAL_STACK_SIZE,
			NULL, tskIDLE_PRIORITY + 3, NULL);

	/* TODO: ... or register to FPGA interrupt */
}
