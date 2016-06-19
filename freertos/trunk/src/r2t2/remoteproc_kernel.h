/* Copyright (C) 2012 Xilinx Inc. */

/*
 * This header defines a number of structs, macros and variables that are used
 * for communication with the Linux Kernel.
 *
 * - RPMSG primitives, and macros
 * - ELF file section definitions
 * - Resource Table types and value macros
 */

#ifndef REMOTEPROC_KERNEL_H
#define REMOTEPROC_KERNEL_H

/* Just load all symbols from Linker script */
extern char *__ring_rx_addr;
#define RING_RX					(unsigned int)&__ring_rx_addr
extern char *__ring_tx_addr;
#define RING_TX					(unsigned int)&__ring_tx_addr

extern char *__ring_tx_addr_used;
#define RING_TX_USED			(unsigned int)&__ring_tx_addr_used
extern char *__ring_rx_addr_used;
#define RING_RX_USED			(unsigned int)&__ring_rx_addr_used

extern char *_start; /* ELF_START should be zero all the time */
#define ELF_START				(unsigned int)&_start
extern char *__elf_end;
#define ELF_END					(unsigned int)&__elf_end

extern char *__trace_buffer_start;
#define TRACE_BUFFER_START		(unsigned int)&__trace_buffer_start
extern char *__trace_buffer_end;
#define TRACE_BUFFER_END		(unsigned int)&__trace_buffer_end

/* This value should be shared with Linker script */
#define TRACE_BUFFER_SIZE		0x8000

/* section helpers */
#define _section(S)			__attribute__((__section__(#S)))
#define __resource				_section(.resource_table)

/* flip up bits whose indices represent features we support */
#define RPMSG_IPU_C0_FEATURES	1

/* virtio ids: keep in sync with the linux "include/linux/virtio_ids.h" */
#define VIRTIO_ID_CONSOLE		3 /* virtio console */
#define VIRTIO_ID_RPMSG			7 /* virtio remote processor messaging */

/* Indices of rpmsg virtio features we support */
#define VIRTIO_RPMSG_F_NS		0 /* RP supports name service notifications */

/* Resource info: Must match include/linux/remoteproc.h: */
#define TYPE_CARVEOUT			0
#define TYPE_DEVMEM				1
#define TYPE_TRACE				2
#define TYPE_VDEV				3
#define TYPE_MMU				4
#define RSC_LAST				5

#define NO_RESOURCE_ENTRIES		13

/*
 * Xilin AddOn
 *
 * 4deaa5f4baae54bb0a7b3d0a9adc6daaa99aef58
 *    ... firmware is designed to have MMU resource type
 *   for firmware purpose.
 */
struct fw_rsc_mmu {
	unsigned int type;
	unsigned int id;
	unsigned int da;
	unsigned int len; /* unused now */
	unsigned int flags;
	char name[32];
};

/**
 * struct fw_rsc_carveout - physically contiguous memory request
 * @da: device address
 * @pa: physical address
 * @len: length (in bytes)
 * @flags: iommu protection flags
 * @reserved: reserved (must be zero)
 * @name: human-readable name of the requested memory region
 *
 * This resource entry requests the host to allocate a physically contiguous
 * memory region.
 *
 * These request entries should precede other firmware resource entries,
 * as other entries might request placing other data objects inside
 * these memory regions (e.g. data/code segments, trace resource entries, ...).
 *
 * Allocating memory this way helps utilizing the reserved physical memory
 * (e.g. CMA) more efficiently, and also minimizes the number of TLB entries
 * needed to map it (in case @rproc is using an IOMMU). Reducing the TLB
 * pressure is important; it may have a substantial impact on performance.
 *
 * If the firmware is compiled with static addresses, then @da should specify
 * the expected device address of this memory region. If @da is set to
 * FW_RSC_ADDR_ANY, then the host will dynamically allocate it, and then
 * overwrite @da with the dynamically allocated address.
 *
 * We will always use @da to negotiate the device addresses, even if it
 * isn't using an iommu. In that case, though, it will obviously contain
 * physical addresses.
 *
 * Some remote processors needs to know the allocated physical address
 * even if they do use an iommu. This is needed, e.g., if they control
 * hardware accelerators which access the physical memory directly (this
 * is the case with OMAP4 for instance). In that case, the host will
 * overwrite @pa with the dynamically allocated physical address.
 * Generally we don't want to expose physical addresses if we don't have to
 * (remote processors are generally _not_ trusted), so we might want to
 * change this to happen _only_ when explicitly required by the hardware.
 *
 * @flags is used to provide IOMMU protection flags, and @name should
 * (optionally) contain a human readable name of this carveout region
 * (mainly for debugging purposes).
 */
struct fw_rsc_carveout {
	unsigned int type;
	unsigned int da;
	unsigned int pa;
	unsigned int len;
	unsigned int flags;
	unsigned int reserved;
	char name[32];
};

/**
 * struct fw_rsc_devmem - iommu mapping request
 * @da: device address
 * @pa: physical address
 * @len: length (in bytes)
 * @flags: iommu protection flags
 * @reserved: reserved (must be zero)
 * @name: human-readable name of the requested region to be mapped
 *
 * This resource entry requests the host to iommu map a physically contiguous
 * memory region. This is needed in case the remote processor requires
 * access to certain memory-based peripherals; _never_ use it to access
 * regular memory.
 *
 * This is obviously only needed if the remote processor is accessing memory
 * via an iommu.
 *
 * @da should specify the required device address, @pa should specify
 * the physical address we want to map, @len should specify the size of
 * the mapping and @flags is the IOMMU protection flags. As always, @name may
 * (optionally) contain a human readable name of this mapping (mainly for
 * debugging purposes).
 *
 * Note: at this point we just "trust" those devmem entries to contain valid
 * physical addresses, but this isn't safe and will be changed: eventually we
 * want remoteproc implementations to provide us ranges of physical addresses
 * the firmware is allowed to request, and not allow firmwares to request
 * access to physical addresses that are outside those ranges.
 */
struct fw_rsc_devmem {
	unsigned int type;
	unsigned int da;
	unsigned int pa;
	unsigned int len;
	unsigned int flags;
	unsigned int reserved;
	char name[32];
};

/**
 * struct fw_rsc_trace - trace buffer declaration
 * @da: device address
 * @len: length (in bytes)
 * @reserved: reserved (must be zero)
 * @name: human-readable name of the trace buffer
 *
 * This resource entry provides the host information about a trace buffer
 * into which the remote processor will write log messages.
 *
 * @da specifies the device address of the buffer, @len specifies
 * its size, and @name may contain a human readable name of the trace buffer.
 *
 * After booting the remote processor, the trace buffers are exposed to the
 * user via debugfs entries (called trace0, trace1, etc..).
 */
struct fw_rsc_trace {
	unsigned int type;
	unsigned int da;
	unsigned int len;
	unsigned int reserved;
	char name[32];
};

/**
 * struct fw_rsc_vdev_vring - vring descriptor entry
 * @da: device address
 * @align: the alignment between the consumer and producer parts of the vring
 * @num: num of buffers supported by this vring (must be power of two)
 * @notifyid is a unique rproc-wide notify index for this vring. This notify
 * index is used when kicking a remote processor, to let it know that this
 * vring is triggered.
 * @reserved: reserved (must be zero)
 *
 * This descriptor is not a resource entry by itself; it is part of the
 * vdev resource type (see below).
 *
 * Note that @da should either contain the device address where
 * the remote processor is expecting the vring, or indicate that
 * dynamically allocation of the vring's device address is supported.
 */
struct fw_rsc_vdev_vring {
	unsigned int da; /* device address */
	unsigned int align;
	unsigned int num;
	unsigned int notifyid;
	unsigned int reserved;
};

/**
 * struct fw_rsc_vdev - virtio device header
 * @id: virtio device id (as in virtio_ids.h)
 * @notifyid is a unique rproc-wide notify index for this vdev. This notify
 * index is used when kicking a remote processor, to let it know that the
 * status/features of this vdev have changes.
 * @dfeatures specifies the virtio device features supported by the firmware
 * @gfeatures is a place holder used by the host to write back the
 * negotiated features that are supported by both sides.
 * @config_len is the size of the virtio config space of this vdev. The config
 * space lies in the resource table immediate after this vdev header.
 * @status is a place holder where the host will indicate its virtio progress.
 * @num_of_vrings indicates how many vrings are described in this vdev header
 * @reserved: reserved (must be zero)
 * @vring is an array of @num_of_vrings entries of 'struct fw_rsc_vdev_vring'.
 *
 * This resource is a virtio device header: it provides information about
 * the vdev, and is then used by the host and its peer remote processors
 * to negotiate and share certain virtio properties.
 *
 * By providing this resource entry, the firmware essentially asks remoteproc
 * to statically allocate a vdev upon registration of the rproc (dynamic vdev
 * allocation is not yet supported).
 *
 * Note: unlike virtualization systems, the term 'host' here means
 * the Linux side which is running remoteproc to control the remote
 * processors. We use the name 'gfeatures' to comply with virtio's terms,
 * though there isn't really any virtualized guest OS here: it's the host
 * which is responsible for negotiating the final features.
 * Yeah, it's a bit confusing.
 *
 * Note: immediately following this structure is the virtio config space for
 * this vdev (which is specific to the vdev; for more info, read the virtio
 * spec). the size of the config space is specified by @config_len.
 */
struct fw_rsc_vdev {
	unsigned int type;
	unsigned int id;
	unsigned int notifyid;
	unsigned int dfeatures;
	unsigned int gfeatures;
	unsigned int config_len;
	char status;
	char num_of_vrings;
	char reserved[2];
};

struct rpmsg_channel_info {
#define RPMSG_NAME_SIZE			32
	char name[RPMSG_NAME_SIZE];
	unsigned int src;
	unsigned int dst;
};

struct rpmsg_hdr {
	unsigned int src;
	unsigned int dst;
	unsigned int reserved;
	unsigned short len;
	unsigned short flags;
	unsigned char data[0];
} __packed;


/* Virtio ring descriptors: 16 bytes.  These can chain together via "next" */
struct vring_desc {
	unsigned int addr; /* Address (guest-physical). */
	unsigned int addr_hi;
	unsigned int len; /* Length. */
	unsigned short flags; /* The flags as indicated above. */
	unsigned short next; /* We chain unused descriptors via this, too */
};

/* unsigned int is used here for ids for padding reasons. */
struct vring_used_elem {
	unsigned int id; /* Index of start of used descriptor chain. */
	unsigned int len; /* Total length of the descriptor chain which was used (written to) */
};

struct vring_used {
	unsigned short flags;
	unsigned short idx;
	struct vring_used_elem ring[];
};

#define VRING_ADDR_MASK				0xffffff
#define VRING_SIZE					256

/* Tx Vring IRQ from Linux */
#define TXVRING_IRQ					11
/* Rx Vring IRQ from Linux */
#define RXVRING_IRQ					12
/* IRQ to notify Linux */
#define NOTIFY_LINUX_IRQ			10

/* vring data buffer max length including the header */
#define PACKET_LEN_MAX				512
#define DATA_LEN_MAX				(PACKET_LEN_MAX - sizeof(struct rpmsg_hdr))

#endif /* REMOTEPROC_KERNEL_H */
