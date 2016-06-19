/* Copyright (C) 2012 Xilinx Inc. */

/*
 * This Header File is common for both the FreeRTOS demo application and the 
 * latencystat user space demo application.
 */

#ifndef LATENCYDEMO_H
#define LATENCYDEMO_H

void message_handler(unsigned char* data, unsigned int len);
void r2t2_init(void);

#endif /* LATENCYDEMO_H */
