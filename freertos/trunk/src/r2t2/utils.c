/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* xilinx includes */
#include "xparameters.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "semphr.h"
#include "xil_exception.h"

#include "xil_printf.h"
#include "xil_cache.h"
#include "xil_cache_l.h"

/* MS: Trace buffer setting */
char *log_buf_base;
unsigned int log_buf_len;
char *current;

void xputs(char *str)
{
	int len = (int)strlen(str);

	if ((current + len) >= (log_buf_base + log_buf_len)) {
		int offset = log_buf_base + log_buf_len - current;
		strncpy(current, str, offset);
		current = log_buf_base;
		strncpy(current, str + offset, len - offset);
	} else {
		strncpy(current, str, len);
		current += len;
	}

	Xil_L1DCacheFlush();
}

void FreeRTOS_ExHandler(void *data);

void freertos_exception_init(void)
{
	Xil_ExceptionInit();

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the ARM processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_UNDEFINED_INT,
	                (Xil_ExceptionHandler)FreeRTOS_ExHandler,
	                (void *)4);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_PREFETCH_ABORT_INT,
	                (Xil_ExceptionHandler)FreeRTOS_ExHandler,
	                (void *)4);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_DATA_ABORT_INT,
	                (Xil_ExceptionHandler)FreeRTOS_ExHandler,
	                (void *)8);
}

void stdio_lock_init(unsigned int base, unsigned int len)
{
    log_buf_base = (char *) base;
    log_buf_len = len;
    current = (char *) base;
}


/* Exception handler (fatal).
 * Attempt to print out a backtrace.
 */
void FreeRTOS_ExHandler(void *data)
{
    unsigned *fp, lr;
    static int exception_count = 0;
    int offset = (int)data;

    xil_printf("\n\rEXCEPTION, HALTED!\n\r");

    fp = (unsigned*)mfgpr(11); /* get current frame pointer */
/*    if (! ptr_valid(fp)) {
        goto spin;
    } */

    /* Fetch Data Fault Address from CP15 */
    lr = mfcp(XREG_CP15_DATA_FAULT_ADDRESS);
    xil_printf("Data Fault Address: 0x%08x\n\r", lr);

    /* The exception frame is built by DataAbortHandler (for example) in
     * FreeRTOS/Source/portable/GCC/Zynq/port_asm_vectors.s:
     * stmdb   sp!,{r0-r3,r12,lr}
     * and the initial handler function (i.e. DataAbortInterrupt() ) in
     * standalone_bsp/src/arm/vectors.c, which is the standard compiler EABI :
     * push    {fp, lr}
     *
     * The relative position of the frame build in port_asm_vectors.s is assumed,
     * as there is no longer any direct reference to it.  If this file (or vectors.c)
     * are modified this location will need to be updated.
     *
     * r0+r1+r2+r3+r12+lr = 5 registers to get to the initial link register where
     * the exception occurred.
     */
    xil_printf("FP: 0x%08x LR: 0x%08x\n\r", (unsigned)fp, *(fp + 5) - offset);
    xil_printf("R0: 0x%08x R1: 0x%08x\n\r", *(fp + 0), *(fp + 1));
    xil_printf("R2: 0x%08x R3: 0x%08x\n\r", *(fp + 2), *(fp + 3));
    xil_printf("R12: 0x%08x\n\r", *(fp + 4));
//spin:
    exception_count++;
    if (exception_count > 1) {
        /* Nested exceptions */
        while (1) {;}
    }

    while (1) {;}
}

/**
 * Remove a specifiec interrupt service routine
 */
void clearIRQhandler(int int_no)
{
	extern XScuGic xInterruptController;

	XScuGic_Disable(&xInterruptController, int_no);
	XScuGic_Disconnect(&xInterruptController, int_no);
}


/**< Pending Clear Register */
#define XSCUGIC_PENDING_CLR_OFFSET	0x00000280

/**
 * Clear the pending interrupt flags
 */
#define XScuGic_ClearPending(DistBaseAddress, Int_Id) \
	XScuGic_WriteReg((u32)(DistBaseAddress), \
			 XSCUGIC_PENDING_CLR_OFFSET + ((Int_Id / 32) * 4), \
			 (1 << (Int_Id % 32)))



/**
 * Setup an interrupt service routine handler
 */
void setupIRQhandler(int int_no, void *fce, void *param)
{
	int Status;
	extern XScuGic xInterruptController;

	clearIRQhandler(int_no);

	// Todo Set priority with XScuGic_SetPriorityTriggerType

	Status = XScuGic_Connect(&xInterruptController, int_no,
	    (Xil_ExceptionHandler)fce, param);
	if (Status != XST_SUCCESS) {
	    return;
	}
	XScuGic_ClearPending(&xInterruptController, int_no);
	XScuGic_Enable(&xInterruptController, int_no);
}

/**
 * Send a software interrupt to a specified cpu core
 */
void swirq_to_linux(int irq, int cpu)
{
	extern XScuGic xInterruptController;

	XScuGic_SoftwareIntr(&xInterruptController, irq, cpu);
}

void dump_data(unsigned char* data, unsigned int len)
{
	unsigned int i;

	for(i = 0 ; i <len ; i++)
	{
		if(i % 16 == 0 && i)
			xil_printf("\r\n");
		else if(i % 2 == 0 && i)
			xil_printf(" ");
		xil_printf("%02x", data[i]);
	}
	xil_printf("\r\n");
}

void routeIRQtoCPU(XScuGic *irqContr, int irq, u8 cpu) {
	u32 mask = 3 << ((irq & 3)*8);
	u32 cval = cpu << ((irq & 3)*8);
	u32 val = XScuGic_DistReadReg(irqContr,  XSCUGIC_SPI_TARGET_OFFSET_CALC(irq)) & ~mask;
	XScuGic_DistWriteReg(irqContr,  XSCUGIC_SPI_TARGET_OFFSET_CALC(irq), val | cval);
}
