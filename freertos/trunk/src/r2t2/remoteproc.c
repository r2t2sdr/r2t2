/* Copyright (C) 2012 Xilinx Inc. */

/*
 * This file contains the implemention of message passing to and from the Linux
 * Kernel, as well has the implementation of Resource Table and MMU setup.
 *
 * - RPMSG tx/rx vring
 * - RPMSG interrupt handling and Linux<->FreeRTOS 'kicks'
 * - MMU Setup and configuration for peripherals
 * - Functions to wrap base message passing primitives for message requests
 */

#include <stdlib.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "xil_cache_l.h"

#include "remoteproc_kernel.h"
#include "remoteproc.h"
#include "utils.h"

int __send_message(u32 src, u32 dst, void *data, u32 len);

/* Linux host needs to know what resources are required by the FreeRTOS
 * firmware.
 *
 * This table is accessed by the kernel during initialisation of the remoteproc
 * driver in order to setup the system for AMP.
 */
struct resource_table {
	unsigned int version;
	unsigned int num;
	unsigned int reserved[2];
	unsigned int offset[NO_RESOURCE_ENTRIES];

	/* text carveout entry */
	struct fw_rsc_carveout text_cout;

	/* rpmsg vdev entry */
	struct fw_rsc_vdev rpmsg_vdev;
	struct fw_rsc_vdev_vring rpmsg_vring0;
	struct fw_rsc_vdev_vring rpmsg_vring1;

	/* trace entry */
	struct fw_rsc_trace trace;
	struct fw_rsc_mmu ttc;
	struct fw_rsc_mmu uart;
	struct fw_rsc_mmu scu;
};

struct resource_table __resource resources = {
	1, /* we're the first version that implements this */
	6, /* number of entries in the table */
	{ 0, 0, }, /* reserved, must be zero */
	/* offsets to entries */
	{
		offsetof(struct resource_table, text_cout),
		offsetof(struct resource_table, rpmsg_vdev),
		offsetof(struct resource_table, trace),
		offsetof(struct resource_table, ttc),
		offsetof(struct resource_table, uart),
		offsetof(struct resource_table, scu),
	},

	/* End of ELF file */
	{ TYPE_CARVEOUT, 0, 0, ELF_END, 0, 0, "TEXT/DATA", },

	/* rpmsg vdev entry */
	{ TYPE_VDEV, VIRTIO_ID_RPMSG, 0, RPMSG_IPU_C0_FEATURES, 0, 0, 0, 2,
			{ 0, 0 }, /* no config data */ },

		/* the two vrings */
		{ RING_TX, 0x1000, VRING_SIZE, 1, 0 },
		{ RING_RX, 0x1000, VRING_SIZE, 2, 0 },

	/* Trace buffer */
	{ TYPE_TRACE, TRACE_BUFFER_START, TRACE_BUFFER_SIZE, 0, "trace_buffer", },

	/* Peripherals */
	{ TYPE_MMU, 0, TTC_BASEADDR, 0, 0xc02, "ttc", },  /* MMU type is not used by Linux, FreeRTOS will handle it in mmu_resource_table_setup */
	{ TYPE_MMU, 1, STDOUT_BASEADDRESS, 0, 0xc02, "uart", },
	{ TYPE_MMU, 2, XPS_SCU_PERIPH_BASE, 0, 0xc02, "scu", },
};

/* This FreeRTOS application address used in the communication with Linux */
#define FREERTOS_APP_ADDR 0x50

/* Linux address to receive service announcement */
#define LINUX_SERVICE_ANNOUNCEMENT_ADDR 0x35

/* Service name. It needs to match the driver name of the corresponding
 * RPMSG driver in Linux. */
#define FREERTOS_APP_SERVICE_NAME "rpmsg-r2t2"

xTaskHandle txVring_handler;
xTaskHandle rxVring_handler;

/* The following variables are to record the TX ring status. 
 * The TX ring is like a round FIFO queue. We use head and tail to 
 * record whether the tx ring queue is full. The queue is full when
 * head == tail.
 * the ring_tx_ready is "1" when Linux site is ready to receive data,
 * it is "0" otherwise. */
static unsigned int ring_tx_used_head = 0;
static unsigned int ring_tx_used_tail = (VRING_SIZE - 1);
static unsigned int ring_tx_ready = 0;

xSemaphoreHandle txring_mutex;
xSemaphoreHandle data_to_lnx_sem;
xSemaphoreHandle data_from_lnx_sem;
xSemaphoreHandle rx_rpmsg_cnt_mutex;
xSemaphoreHandle tx_rpmsg_cnt_mutex;

/* Application callback function pointer */
datagram_hanlder_cb* dtgrm_handler = NULL;

/* Virtio address to linux */
static unsigned int lnx_virtio_add = 0;

/* Amount of rpmsg to received/send
 * FIXME: Lock?
 */
static unsigned int rx_rpmsg_cnt = 0;
static unsigned int tx_rpmsg_cnt = 0;

/* R2T2 rpmessage types */
typedef enum {
    SET_VIRTIO_ADD = 0, // Set Linux virtio address
    CLEAR_VIRTIO_ADD,   // Clear Linux virtio address
    NEW_DATAGRAM        // New datagram data
} r2t2_rpmsg_types;

/* FIXME: Schipstreck compiler: sizeof(r2t2_rpmsg_types) = 1 and 4 under Linux */
#define R2T2_RPMGS_TYPES_SIZE 4

/* -------------------------------------------------------------------------- */
/* Function to receive message from Linux from rxvring. */
void read_message(void)
{
	static unsigned long empty_msg_cnt = 0;
	unsigned int i;
	struct vring_used volatile *ring_rx_used = (void *)RING_RX_USED;
	unsigned int index = ring_rx_used->idx % VRING_SIZE;
	struct vring_desc volatile *ring_rx = (void *)RING_RX;
	struct rpmsg_hdr *hdr = (struct rpmsg_hdr *)(ring_rx[index].addr &
			VRING_ADDR_MASK);
	unsigned char *rpmsg_data;
	r2t2_rpmsg_types msg_type = 0;

	/* Update index */
	ring_rx_used->ring[index].id = index;
	ring_rx_used->ring[index].len = PACKET_LEN_MAX;
	ring_rx_used->idx += 1; // last index 0 keep increasing
	if(!ring_rx[index].addr)
	{
		xil_printf("!!!!!! 0x%02x VRING @ is NULL!!!!!!\r\n", ++empty_msg_cnt);
		return;
	}

	/* Read the message type */
	for(i = 0 ; i < R2T2_RPMGS_TYPES_SIZE ; i++)
		msg_type |= hdr->data[i] << (i * 8);
	rpmsg_data = hdr->data + R2T2_RPMGS_TYPES_SIZE;

	/* Perform the right action depending on message type */
	if(msg_type == SET_VIRTIO_ADD)
	{
		vPortEnterCritical();
		for(i = 0 ; i < sizeof(lnx_virtio_add) ; i++)
			lnx_virtio_add |= rpmsg_data[i] << (i * 8);
		xil_printf("Linux virtio address set to 0x%02x\r\n", lnx_virtio_add);
		vPortExitCritical();
	}
	else if(msg_type == CLEAR_VIRTIO_ADD)
	{
		vPortEnterCritical();
		lnx_virtio_add = 0;
		xil_printf("Linux virtio address cleared!\r\n");
		vPortExitCritical();
	}
	else if(msg_type == NEW_DATAGRAM)
	{
		if(dtgrm_handler)
			dtgrm_handler(rpmsg_data, hdr->len - R2T2_RPMGS_TYPES_SIZE);
	}
	else
	{
		xil_printf("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n"
				"Unknown R2T2 rpmessage type 0x%x\r\n", msg_type);
		dump_data(hdr->data, hdr->len);
		xil_printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\r\n\r\n");
	}
}

/*
 * Function to send messages to Linux through txvring.
 * It will not return until it sends successfully.
 * @para:
 *  src: source address of the remote processor message
 *  dst: destination address of the remote processor message
 *  data: data of the message
 *  len: length of the data
 * @return:
 *  void
 */
void block_send_message(u32 src, u32 dst, void *data, u32 len)
{
	while(__send_message(src, dst, data , len)) {
		portTickType xNextWakeTime;
		/* Initialise xNextWakeTime */
		xNextWakeTime = xTaskGetTickCount();
		vTaskDelayUntil( &xNextWakeTime, 1 / portTICK_RATE_MS );
	}
}

/* -------------------------------------------------------------------------- */
/* Mutex lock/unlock */

static void lock_txring_mutex()
{
	while (xSemaphoreTake(txring_mutex, portMAX_DELAY) == pdFALSE)
		;
}
static void unlock_txring_mutex()
{
	xSemaphoreGive(txring_mutex);
}

/* -------------------------------------------------------------------------- */
/* irq handlers */

void txvring_irq2(void *data)
{
	static BaseType_t xHigherPriorityTaskWoken;

	/* Linux kick since it is ready for data */
	xSemaphoreTakeFromISR(tx_rpmsg_cnt_mutex, &xHigherPriorityTaskWoken);
	tx_rpmsg_cnt++;
	xSemaphoreGiveFromISR(tx_rpmsg_cnt_mutex, &xHigherPriorityTaskWoken);
	xSemaphoreGiveFromISR(data_to_lnx_sem, &xHigherPriorityTaskWoken);
}

static void txvring_task( void *pvParameters )
{
	int has_msg;
	typedef enum {
		SERVICE_ANNOUNCE = 0,
		RUNNING,
	} state_machine;

	state_machine state = SERVICE_ANNOUNCE;
	struct rpmsg_channel_info data;

	struct vring_used volatile *ring_tx_used = (void *)RING_TX_USED;

	for( ;; ) {
		/* Enter a critical section, for atomicity */
		if (xSemaphoreTake( data_to_lnx_sem, portMAX_DELAY ) == pdTRUE) {

			if(xSemaphoreTake(tx_rpmsg_cnt_mutex, portMAX_DELAY) != pdTRUE)
				continue;
			has_msg = tx_rpmsg_cnt > 0;
			xSemaphoreGive(tx_rpmsg_cnt_mutex);

			while(has_msg)
			{

				/* Linux expects to get message*/
				switch(state) {
					case SERVICE_ANNOUNCE:
						lock_txring_mutex();
						ring_tx_ready = 1;
						unlock_txring_mutex();

						memset(&data, 0, sizeof(data));

						data.src = FREERTOS_APP_ADDR;
						data.dst = 0;
						strncpy(data.name, FREERTOS_APP_SERVICE_NAME, RPMSG_NAME_SIZE);

						block_send_message(FREERTOS_APP_ADDR,
								LINUX_SERVICE_ANNOUNCEMENT_ADDR, &data, sizeof(data));
						state = RUNNING;
						break;
					case RUNNING:
						lock_txring_mutex();
						ring_tx_used_tail++; /* Linux has released a buffer */
						if (ring_tx_used->idx != ring_tx_used_head) {
							/* If there is message pending in the TX ring,
							 * send it. */
							ring_tx_used->idx += 1;
							Xil_L1DCacheFlush();
							/* Kick Linux since it is ready for data */
							swirq_to_linux(NOTIFY_LINUX_IRQ, 1);
						} else {
							ring_tx_ready = 1;
						}
						unlock_txring_mutex();
						break;
					default:
						xil_printf("Unknown state\r\n");
						break;
				}

				if(xSemaphoreTake(tx_rpmsg_cnt_mutex, portMAX_DELAY) != pdTRUE)
					break;
				has_msg = --tx_rpmsg_cnt > 0;
				xSemaphoreGive(tx_rpmsg_cnt_mutex);
			}
		}
	}
}

void rxvring_irq3(void *data)
{
	static BaseType_t aHigherPriorityTaskWoken;

	/* Enter a critical section, for atomicity */
	xSemaphoreTakeFromISR(rx_rpmsg_cnt_mutex, &aHigherPriorityTaskWoken);
	rx_rpmsg_cnt++;
	xSemaphoreGiveFromISR(rx_rpmsg_cnt_mutex, &aHigherPriorityTaskWoken);
	xSemaphoreGiveFromISR(data_from_lnx_sem, &aHigherPriorityTaskWoken);
}

static void rxvring_task( void *pvParameters )
{
	int has_msg;
	for( ;; ) {
		if (xSemaphoreTake( data_from_lnx_sem, portMAX_DELAY ) == pdTRUE)
		{
			if(xSemaphoreTake(rx_rpmsg_cnt_mutex, portMAX_DELAY) != pdTRUE)
				continue;
			has_msg = rx_rpmsg_cnt > 0;
			xSemaphoreGive(rx_rpmsg_cnt_mutex);

			while(has_msg)
			{
				read_message();

				if(xSemaphoreTake(rx_rpmsg_cnt_mutex, portMAX_DELAY) != pdTRUE)
					break;
				has_msg = --rx_rpmsg_cnt > 0;
				xSemaphoreGive(rx_rpmsg_cnt_mutex);
			}
		}
	}
}

/* -------------------------------------------------------------------------- */

/*
 * Function to send messages to Linux through txvring.
 * @para:
 *  src: source address of the remote processor message
 *  dst: destination address of the remote processor message
 *  data: data of the message
 *  len: length of the data
 * @return:
 *  0: succeeded
 *  1: failed
 */
int __send_message(u32 src, u32 dst, void *data, u32 len)
{
	struct vring_used volatile *ring_tx_used = (void *)RING_TX_USED;
	struct vring_desc volatile *ring_tx = (void *)RING_TX;
	
	unsigned int index;
	
	lock_txring_mutex();
	index = ring_tx_used_head % VRING_SIZE;
	if (index == (ring_tx_used_tail % VRING_SIZE)) {
		unlock_txring_mutex();
		//xil_printf("Vring TX is full\r\n");
		return -1;
	}
	ring_tx_used_head++;
	unlock_txring_mutex();
	struct rpmsg_hdr *hdr = (struct rpmsg_hdr *)(ring_tx[index].addr &
			VRING_ADDR_MASK);

	/* Check if data size is greater than packed size - if yes, send just part
	 * of it */
	len = len > DATA_LEN_MAX ? DATA_LEN_MAX : len;

	/* Clear the whole message */
	//memset(hdr, 0, (char)PACKET_LEN_MAX);
	hdr->src = src;
	hdr->dst = dst;
	hdr->reserved = 0;
	hdr->flags = 0;
	hdr->len = (unsigned short)len; // data len
	memcpy(&hdr->data, data, hdr->len);

	ring_tx_used->ring[index].id = index;
	ring_tx_used->ring[index].len = PACKET_LEN_MAX;

	lock_txring_mutex();
	if (ring_tx_ready) {
		/* Since Linux uses idx to get the buffer sent by FreeRTOS
		 * We should not modify this TX used ring's idx until Linux
		 * is ready to accept new data. */
		ring_tx_used->idx += 1; 
		Xil_L1DCacheFlush();
		/* Kick Linux since it is ready to accept data */
		swirq_to_linux(NOTIFY_LINUX_IRQ, 1);
		ring_tx_ready--;
	} else {
		/* Linux is not ready. Just update the buffer content,
		 * but not kick Linux */
		Xil_L1DCacheFlush();
	}
	unlock_txring_mutex();
	return 0;
}

/* -------------------------------------------------------------------------- */
void remoteproc_send_data(unsigned char* data, unsigned int len)
{
	unsigned int dest = 0;
	vPortEnterCritical();
	if(lnx_virtio_add)
		dest = lnx_virtio_add;
	vPortExitCritical();

	/* Send data */
	if (dest && data != NULL && len > 0) {
		int total = len;
		int tmpsize = 0;
		int sum = 0;

		/* Segment the transfer into 'DATA_LEN_MAX' size chunks */
		for (; sum < total; ) {
			tmpsize = (total - sum) <= DATA_LEN_MAX ? (total - sum) :
					DATA_LEN_MAX;
			block_send_message(FREERTOS_APP_ADDR, dest,
					(char *)(data + sum), tmpsize);
			sum += tmpsize;
		}
	}
	else
		xil_printf("No data sent!\r\n");
}

/* -------------------------------------------------------------------------- */
/* Setup Function */
void remoteproc_init(datagram_hanlder_cb* handler)
{
	dtgrm_handler = handler;

	/* Create semaphores and mutexes */
	vSemaphoreCreateBinary( txring_mutex );
	if (txring_mutex == NULL) {
		xil_printf("ERROR: Failed to create TX Ring mutext!\r\n");
		return;
	}

	data_to_lnx_sem = xSemaphoreCreateCounting( 1, 0 );
	if (data_to_lnx_sem == NULL) {
		xil_printf("ERROR: Failed to create data to linux semaphore!\r\n");
		return;
	}

	data_from_lnx_sem = xSemaphoreCreateCounting( 1, 0 );
	if (data_from_lnx_sem == NULL) {
		xil_printf("ERROR: Failed to create data from linux semaphore!\r\n");
		return;
	}

	rx_rpmsg_cnt_mutex = xSemaphoreCreateMutex();
	if (rx_rpmsg_cnt_mutex == NULL) {
		xil_printf("ERROR: Failed to create rx message count mutex!\r\n");
		return;
	}

	tx_rpmsg_cnt_mutex = xSemaphoreCreateMutex();
	if (tx_rpmsg_cnt_mutex == NULL) {
		xil_printf("ERROR: Failed to create tx message count mutex!\r\n");
		return;
	}

	setupIRQhandler(TXVRING_IRQ, &txvring_irq2, NULL);
	setupIRQhandler(RXVRING_IRQ, &rxvring_irq3, NULL);

	/* Setup tx/rx vring processing tasks */
	xTaskCreate( txvring_task, "TXVRING_TASK",
			configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3,
			&txVring_handler);
	xTaskCreate( rxvring_task, "RXVRING_TASK",
			configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3,
			&rxVring_handler);
}

/* Setup the Trace Buffer */
void trace_init(void)
{
	stdio_lock_init(TRACE_BUFFER_START, TRACE_BUFFER_SIZE);
}

/* -------------------------------------------------------------------------- */
/* Resource Setup Functions */

void enable_tlb(unsigned int addr, unsigned int flags)
{
	extern char *MMUTable;
	unsigned int link;

	addr = addr & ~0xFFFFF; /* Address must be 1MB aligned */
	link = (u32)&MMUTable + ((addr / 0x100000) * 4);

	*(u32 *)link = addr | flags;
	Xil_L1DCacheFlush();
}

void disable_tlb(unsigned int addr)
{
	extern char *MMUTable;
	unsigned int link;

	addr = addr & ~0xFFFFF; /* Address must be 1MB aligned */
	link = (u32)&MMUTable + ((addr / 0x100000) * 4);

	*(u32 *)link = addr | 0x000;
	Xil_L1DCacheFlush();
}

void mmu_resource_table_setup(void)
{
	int i;

	if (resources.version == 1) {
		unsigned char *ptr = (unsigned char *)&resources;

		for (i = 0; i < resources.num; i++) {
			int offset = resources.offset[i];
			unsigned int type = *(unsigned int *)(ptr + offset);
			if (offset && (type == TYPE_MMU)) {
				struct fw_rsc_mmu *mmu = (struct fw_rsc_mmu *)(ptr + offset);
				enable_tlb(mmu->da, mmu->flags);
			}
		}
	}

	extern char *MMUTable;
#ifdef DEBUG
	return;
#endif

	/* This is the most important lines - disable access to MMU table
	 * to avoid currupting Linux */
	disable_tlb((u32)&MMUTable);
	Xil_L1DCacheFlush();
	/* Flush all TLBs */
	__asm__ __volatile__("dsb; mov	%0,#0; \
			mcr	p15, 0, r0, c8, c7, 0;" \
				: "=r" (i));
}
