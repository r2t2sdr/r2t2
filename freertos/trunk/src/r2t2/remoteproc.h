/* Copyright (C) 2012 Xilinx Inc. */

#ifndef REMOTEPROC_H
#define REMOTEPROC_H

/* TTC1 base address, mapped by the remoteproc resource initialization */
#ifndef TTC_BASEADDR
#define TTC_BASEADDR 0XF8002000
#endif

/* Resource table setup */
void mmu_resource_table_setup(void);

/* Mask for the state field which is stored as the first word in each message */
#define REMOTEPROC_REQUEST_ACK_MASK			0x80000000

typedef void (datagram_hanlder_cb)(unsigned char* data, unsigned int len);

/* trace buffer init function */
void trace_init(void);

/* Remoteproc init functions */
void remoteproc_init(datagram_hanlder_cb* handler);

/* Send data with remoteproc */
void remoteproc_send_data(unsigned char* data, unsigned int len);

#endif /* REMOTEPROC_H */
