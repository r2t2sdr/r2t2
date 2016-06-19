#ifndef _UTILS_H_
#define _UTILS_H_

#include "xscugic.h"

void freertos_exception_init(void);
void stdio_lock_init(unsigned int base, unsigned int len);

void xputs(char *str);
void safe_printf(const char *format, ...);
void setupIRQhandler(int int_no, void *fce, void *param);
void register_handler(void *handler_priv);
void swirq_to_linux(int irq, int cpu);
void clearIRQhandler(int int_no);

void dump_data(unsigned char* data, unsigned int len);
void routeIRQtoCPU(XScuGic *irqContr, int irq, u8 cpu);

/* trace() prints to trace buffer */
#define trace(x)		xputs(x)

/* console() prints to serial device */
#define console(x)		xil_printf(x)

/* Set the log to trace or console */
#define log(x)			console(x)


#endif
