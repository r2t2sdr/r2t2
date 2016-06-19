#undef   DEBUG

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/dmaengine.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/random.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/amba/xilinx_dma.h>
#include <linux/dmapool.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

#define RAD_NETDEV_NAME "rad%d"
#define DRIVER_NAME 	radium

#define MAC_ADDR 		"\x02\x00r2t2"
#define ETH_P_R2T2 		0x7232
#define PADDING_SIZE 	2
#define HEADER_LEN  	16

#define DESC_CNT 		24	

/* Hw specific definitions */
#define XILINX_DMA_MAX_CHANS_PER_DEVICE	0x2 /* Max no of channels */
#define XILINX_DMA_MAX_TRANS_LEN	0x7FFFFF /* Max transfer length */

/* Register Offsets */
#define XILINX_DMA_CONTROL_OFFSET	0x00 /* Control Reg */
#define XILINX_DMA_STATUS_OFFSET	0x04 /* Status Reg */
#define XILINX_DMA_CDESC_OFFSET		0x08 /* Current descriptor Reg */
#define XILINX_DMA_TDESC_OFFSET		0x10 /* Tail descriptor Reg */
#define XILINX_DMA_SRCADDR_OFFSET	0x18 /* Source Address Reg */
#define XILINX_DMA_DSTADDR_OFFSET	0x20 /* Dest Address Reg */
#define XILINX_DMA_BTT_OFFSET		0x28 /* Bytes to transfer Reg */

/* General register bits definitions */
#define XILINX_DMA_CR_CYCLIC_MASK	0x00000010 /* Cyclic BD enable */
#define XILINX_DMA_CR_RESET_MASK	0x00000004 /* Reset DMA engine */
#define XILINX_DMA_CR_RUNSTOP_MASK	0x00000001 /* Start/stop DMA engine */

#define XILINX_DMA_SR_HALTED_MASK	0x00000001 /* DMA channel halted */
#define XILINX_DMA_SR_IDLE_MASK		0x00000002 /* DMA channel idle */
#define XILINX_DMA_SR_CMP_MASK		0x80000000 /* Compleated */

#define XILINX_DMA_XR_IRQ_IOC_MASK	0x00001000 /* Completion interrupt */
#define XILINX_DMA_XR_IRQ_DELAY_MASK	0x00002000 /* Delay interrupt */
#define XILINX_DMA_XR_IRQ_ERROR_MASK	0x00004000 /* Error interrupt */
#define XILINX_DMA_XR_IRQ_ALL_MASK	0x00007000 /* All interrupts */

#define XILINX_DMA_XR_DELAY_MASK	0xFF000000 /* Delay timeout counter */
#define XILINX_DMA_XR_COALESCE_MASK	0x00FF0000 /* Coalesce counter */

#define XILINX_DMA_DELAY_SHIFT		24 /* Delay timeout counter shift */
#define XILINX_DMA_COALESCE_SHIFT	16 /* Coalesce counter shift */

#define XILINX_DMA_DELAY_MAX		0xFF /* Maximum delay counter value */
#define XILINX_DMA_COALESCE_MAX		0xFF /* Max coalescing counter value */

#define XILINX_DMA_RX_CHANNEL_OFFSET	0x30 /* S2MM Channel Offset */

#define XLINX_DMA_DESC_STATUS_LEN_MASK 	0x003FFFFF

/* BD definitions for AXI Dma */
#define XILINX_DMA_BD_STS_ALL_MASK	0xF0000000
#define XILINX_DMA_BD_SOP		0x08000000 /* Start of packet bit */
#define XILINX_DMA_BD_EOP		0x04000000 /* End of packet bit */

/* Feature encodings */
#define XILINX_DMA_FTR_HAS_SG		0x00000100 /* Has SG */
#define XILINX_DMA_FTR_HAS_SG_SHIFT	8 /* Has SG shift */
/* Optional feature for dma */
#define XILINX_DMA_FTR_STSCNTRL_STRM	0x00010000

/* Delay loop counter to prevent hardware failure */
#define XILINX_DMA_RESET_LOOP		1000000
#define XILINX_DMA_HALT_LOOP		1000000

static int RADIUM_DMA_BUF_SIZE = 1024*4;

/* The instantiated net device */
static struct net_device* netdev_rad;

/* rad net device private data */
struct netdev_rad_private{
	struct net_device *net_device;
};

static struct xilinx_dma_device *xdev;
static struct net_device *tx_ndev;

/* Hardware descriptor */
struct xilinx_dma_desc_hw {
	u32 next_desc;	/* 0x00 */
	u32 pad1;	/* 0x04 */
	u32 buf_addr;	/* 0x08 */
	u32 pad2;	/* 0x0C */
	u32 pad3;	/* 0x10 */
	u32 pad4;	/* 0x14 */
	u32 control;	/* 0x18 */
	u32 status;	/* 0x1C */
	u32 app_0;	/* 0x20 */
	u32 app_1;	/* 0x24 */
	u32 app_2;	/* 0x28 */
	u32 app_3;	/* 0x2C */
	u32 app_4;	/* 0x30 */
} __aligned(64);

/* Software descriptor */
struct xilinx_dma_desc_sw {
	struct xilinx_dma_desc_hw hw;
	struct xilinx_dma_desc_sw *next;
	dma_addr_t phys; 			// physical address of hw desc in memory
	dma_addr_t dma_buf; 		// physical address of buffer 
	u8* buf; 					// pointer to mmap buffer
	//struct list_head node;
} __aligned(64);


/* Per DMA specific operations should be embedded in the channel structure */
struct xilinx_dma_chan {
	void __iomem *regs;		/* Control status registers */
	struct list_head desc_list;	/* descriptors */
	struct dma_chan common;		/* DMA common channel */
	struct dma_pool *desc_pool;	/* Descriptors pool */
	spinlock_t lock;		/* Descriptor operation lock */
	struct xilinx_dma_desc_sw *desc;
	struct device *dev;		/* The dma device */
	int irq;			/* Channel IRQ */
	int id;				/* Channel ID */
	enum dma_transfer_direction direction;
					/* Transfer direction */
	int max_len;			/* Maximum data len per transfer */
	bool has_sg;			/* Support scatter transfers */
	bool has_dre;			/* Support unaligned transfers */
	int err;			/* Channel has errors */
	struct tasklet_struct tasklet;	/* Cleanup work after irq */
	u32 feature;			/* IP feature */
	u32 private;			/* Match info for channel request */
	void (*start_transfer)(struct xilinx_dma_chan *chan);
					/* Device configuration info */
};

/* DMA Device Structure */
struct xilinx_dma_device {
	void __iomem *regs;
	struct device *dev;
	struct dma_device common;
	struct xilinx_dma_chan *chan[XILINX_DMA_MAX_CHANS_PER_DEVICE];
	u32 feature;
};

//------------------------------------------------------------------------------
static int rad_prepare_rx(void);
static int rad_open(struct net_device *net_dev) {
	// struct netdev_rad_private* priv = netdev_priv(net_dev);

	dev_dbg(&net_dev->dev, "%s\n", __func__);

	netif_start_queue(net_dev);
	rad_prepare_rx();
	return 0;
}
//------------------------------------------------------------------------------
//
static inline void dma_write(struct xilinx_dma_chan *chan, u32 reg, u32 val) {

//	dev_err(chan->dev, "write chan: %x reg %x %x\n", (unsigned int)chan, chan->regs+reg, val);
	writel(val, chan->regs + reg);
}

static inline u32 dma_read(struct xilinx_dma_chan *chan, u32 reg) {
	return readl(chan->regs + reg);
}

static struct xilinx_dma_chan *getChan(enum dma_transfer_direction dir) {
	int i;
	for (i=0;i<XILINX_DMA_MAX_CHANS_PER_DEVICE;i++)
		if (xdev->chan && xdev->chan[i]->direction == dir)
			return xdev->chan[i];
	return NULL;
}

static void wait_dma_start(struct xilinx_dma_chan *chan)
{
	int loop = XILINX_DMA_HALT_LOOP;

	/* Wait for the hardware to start */
	while (loop) {
		if ((dma_read(chan, XILINX_DMA_STATUS_OFFSET) & XILINX_DMA_CR_RUNSTOP_MASK)==0)
			break;
		loop -= 1;
	}

	if (!loop) {
		pr_debug("Cannot start channel %x: %x\n",
			 (unsigned int)chan, (unsigned int)dma_read(chan, XILINX_DMA_CONTROL_OFFSET));
		chan->err = 1;
	}
}

static void wait_dma_stop(struct xilinx_dma_chan *chan)
{
	int loop = XILINX_DMA_HALT_LOOP;

	/* Wait for the hardware to start */
	while (loop) {
		if ((dma_read(chan, XILINX_DMA_STATUS_OFFSET) & XILINX_DMA_CR_RUNSTOP_MASK)==1)
			break;
		loop -= 1;
	}

	if (!loop) {
		pr_debug("Cannot stop channel %x: %x\n",
			 (unsigned int)chan, (unsigned int)dma_read(chan, XILINX_DMA_CONTROL_OFFSET));
		chan->err = 1;
	}
}

//------------------------------------------------------------------------------

/* Reset hardware */
static int dma_reset(struct xilinx_dma_chan *chan)
{
	int loop = XILINX_DMA_RESET_LOOP;
	u32 tmp;

	dma_write(chan, XILINX_DMA_CONTROL_OFFSET,
		  dma_read(chan, XILINX_DMA_CONTROL_OFFSET) | XILINX_DMA_CR_RESET_MASK);

	tmp = dma_read(chan, XILINX_DMA_CONTROL_OFFSET) & XILINX_DMA_CR_RESET_MASK;

	/* Wait for the hardware to finish reset */
	while (loop && tmp) {
		tmp = dma_read(chan, XILINX_DMA_CONTROL_OFFSET) & XILINX_DMA_CR_RESET_MASK;
		loop -= 1;
	}

	if (!loop) {
		dev_err(chan->dev, "reset timeout, cr %x, sr %x\n",
			dma_read(chan, XILINX_DMA_CONTROL_OFFSET),
			dma_read(chan, XILINX_DMA_STATUS_OFFSET));
		return -EBUSY;
	}

	return 0;
}

static int rad_prepare_rx(void) {
	struct xilinx_dma_desc_sw *desc;
	struct xilinx_dma_chan *chan = getChan(DMA_DEV_TO_MEM);

	desc = chan->desc;

	//dma_write(chan, XILINX_DMA_CONTROL_OFFSET, 0);
	//wait_dma_stop(chan);

	desc->hw.status = 0;

	dma_write(chan, XILINX_DMA_CDESC_OFFSET, desc->phys);
	dma_write(chan, XILINX_DMA_CONTROL_OFFSET, XILINX_DMA_CR_RUNSTOP_MASK );	
	wait_dma_start(chan);

	dma_write(chan, XILINX_DMA_CONTROL_OFFSET, dma_read(chan, XILINX_DMA_CONTROL_OFFSET) | XILINX_DMA_CR_CYCLIC_MASK |  XILINX_DMA_XR_IRQ_ALL_MASK| (1 << XILINX_DMA_COALESCE_SHIFT));
	dma_write(chan, XILINX_DMA_TDESC_OFFSET, 0);

	return 0;
}

static void debug_desc(char* comment, struct xilinx_dma_chan *chan) {
	struct xilinx_dma_desc_sw *desc;
	u8 *buf;
	int i;
	static int cnt=0;

#ifndef DEBUG
	return;
#endif

	if (cnt++<1000)
		return;
	cnt=0;

	desc = chan->desc;

	pr_debug ("%s %i: c %08x s %08x cd: %08x td: %08x\n",
			comment,
			chan->direction,
			(unsigned int)dma_read(chan, XILINX_DMA_CONTROL_OFFSET),
			(unsigned int)dma_read(chan, XILINX_DMA_STATUS_OFFSET),
			(unsigned int)dma_read(chan, XILINX_DMA_CDESC_OFFSET),
			(unsigned int)dma_read(chan, XILINX_DMA_TDESC_OFFSET));

	for (i=0;i<DESC_CNT;i++) {
		buf = desc->buf;
		pr_debug( "    desbuf: %08x hw: %08x next %08x buf %08x status %08x cont %08x buf: %02x%02x%02x%02x\n",
				(unsigned int)desc->buf,
				(unsigned int)&desc->hw,
				(unsigned int)desc->hw.next_desc,
				(unsigned int)desc->hw.buf_addr,
				(unsigned int)desc->hw.status,
				(unsigned int)desc->hw.control,
				buf[0],buf[1],buf[2],buf[3]
				);

		desc = desc->next;
	}
}

static int rad_xmit(struct sk_buff *skb, struct net_device *net_dev) {
	// struct netdev_rad_private* priv = netdev_priv(net_dev);
	int ret = NETDEV_TX_OK;
	u8* buf;
	struct xilinx_dma_desc_sw *desc;
	struct xilinx_dma_chan *chan = getChan(DMA_MEM_TO_DEV);

	desc = chan->desc;
	buf = desc->buf;
	tx_ndev = net_dev;

#if 0
	printk ("rx packet: %i %x %x %x %x\n",skb->len, skb->data[16],skb->data[17],skb->data[18],skb->data[19]);
	goto error;
#endif

	if ((desc->hw.status & XILINX_DMA_SR_CMP_MASK) == 0) {
		pr_debug("radium: tx dma busy, packet ignored");
		if (!netif_queue_stopped(net_dev))
			netif_stop_queue(net_dev);

		return NETDEV_TX_BUSY;
	}

	// TODO unbenutzen descriptor suchen
	dma_write(chan, XILINX_DMA_CONTROL_OFFSET, 0);
	wait_dma_stop(chan);

	if (skb->len > RADIUM_DMA_BUF_SIZE - HEADER_LEN) {
		printk(KERN_ERR "radium: packet to long, ignored");
		goto error;
	}
	memcpy(buf, skb->data + HEADER_LEN, skb->len - HEADER_LEN);

	desc->hw.control = (skb->len - HEADER_LEN) | XILINX_DMA_BD_SOP	| XILINX_DMA_BD_EOP; 
	desc->hw.status = 0;

	// start transfer
	dma_write(chan, XILINX_DMA_CDESC_OFFSET, desc->phys);
	dma_write(chan, XILINX_DMA_CONTROL_OFFSET, XILINX_DMA_CR_RUNSTOP_MASK);	
	wait_dma_start(chan);

	dma_write(chan, XILINX_DMA_CONTROL_OFFSET, 
			dma_read(chan, XILINX_DMA_CONTROL_OFFSET) | XILINX_DMA_XR_IRQ_ALL_MASK | (1 << XILINX_DMA_COALESCE_SHIFT));	

	// and go ..
	dma_write(chan, XILINX_DMA_TDESC_OFFSET, desc->phys);

	net_dev->stats.tx_bytes += skb->len - HEADER_LEN;
	net_dev->stats.tx_packets++;

error:
	dev_kfree_skb(skb);
	return ret;
}

//------------------------------------------------------------------------------
static int rad_stop(struct net_device *net_dev) {
	// struct netdev_rad_private* priv = netdev_priv(net_dev);

	dev_dbg(&net_dev->dev, "%s\n", __func__);
	return 0;
}


static void dma_do_tasklet(unsigned long data)
{
	struct sk_buff *skb;
	struct xilinx_dma_chan *chan;
	struct xilinx_dma_desc_sw *desc;
	u8 *from;
	u16 ethType;
	int i,len;
	static struct xilinx_dma_desc_sw *last_desc=NULL;
	int compl = 0;
	static int rx=0;
	
	chan = (struct xilinx_dma_chan *)data;

	desc = chan->desc;
	if (last_desc)
		desc = last_desc;

	if (chan->direction == DMA_DEV_TO_MEM) {
		// search for completed descriptors
		for (i=0;i<DESC_CNT;i++) {
			if ((desc->hw.status & XILINX_DMA_SR_CMP_MASK)) {

				from = desc->buf;
				len = desc->hw.status & XLINX_DMA_DESC_STATUS_LEN_MASK;


#if 0
				if (desc->hw.status & XILINX_DMA_BD_SOP) {
					if ((from[3] & 15) == 8) {
						writel(readl((u32*)(0x50010000 + 0x00010000*(rx>1))),(u32*)0x50090004);
						rx++;
						rx &= 15;
						from[3] |= rx>>1;
					}
				} 
				//if (((from[3] & 15) == 8) && ((rx & 1) == 0))
				//	continue;
#endif

				// send skb
				// skb = dev_alloc_skb(len + PADDING_SIZE + NET_IP_ALIGN);
				skb = dev_alloc_skb(RADIUM_DMA_BUF_SIZE);
				if(skb == NULL) {
					printk(KERN_ERR "Cannot allocate new skb\n");
					return;
				}
				// generate dummy ethernet header
				memcpy(skb_put(skb, 6), MAC_ADDR, 6);        // to mac
				memcpy(skb_put(skb, 6), "\0\0\0\0\0\0", 6);  // from mac
				ethType = htons(ETH_P_R2T2); 
				memcpy(skb_put(skb, 2), &ethType, 2); // R2T2 ethernet type
				memcpy(skb_put(skb, PADDING_SIZE), "\0\0", PADDING_SIZE);   // padding

				// mark start and end of package in data
				if (desc->hw.status & XILINX_DMA_BD_SOP)
					from[3] |= 0x10;
				if (desc->hw.status & XILINX_DMA_BD_EOP)
					from[3] |= 0x20;

				memcpy(skb_put(skb, len), from, len);

				skb->dev = netdev_rad;
				skb->protocol = eth_type_trans(skb, netdev_rad);
				skb->ip_summed = CHECKSUM_UNNECESSARY;

				// Send the skb in a workqueue
				netif_rx_ni(skb);

				netdev_rad->stats.rx_bytes += len;
				netdev_rad->stats.rx_packets++;
				
				// clear completion
				desc->hw.status &=  ~XILINX_DMA_SR_CMP_MASK;

				last_desc = desc;

				compl++;
			}

			desc = desc->next;
		}
		if (compl==DESC_CNT)
			pr_debug("DMA-Descr overrun\n");

		// !!!!! local loop
		// rad_xmit(skb,NULL);
	} else {
		netif_wake_queue(tx_ndev);
	}
}

static irqreturn_t dma_intr_handler(int irq, void *data)
{
	struct xilinx_dma_chan *chan = data;
	u32 stat, reg;
	struct xilinx_dma_desc_sw *desc;


	debug_desc("intr",chan);
	desc = chan->desc;

	reg = dma_read(chan, XILINX_DMA_CONTROL_OFFSET);

	/* Disable intr */
	dma_write(chan, XILINX_DMA_CONTROL_OFFSET, reg & ~XILINX_DMA_XR_IRQ_ALL_MASK);

	stat = dma_read(chan, XILINX_DMA_STATUS_OFFSET);
	if (!(stat & XILINX_DMA_XR_IRQ_ALL_MASK))
		return IRQ_NONE;

	/* Ack the interrupts */
	dma_write(chan, XILINX_DMA_STATUS_OFFSET,
		  XILINX_DMA_XR_IRQ_ALL_MASK);

	/* Check for only the interrupts which are enabled */
	stat &= (reg & XILINX_DMA_XR_IRQ_ALL_MASK);

	if (stat & XILINX_DMA_XR_IRQ_ERROR_MASK) {
		dev_err(chan->dev,
			"Channel %x has errors %x, cdr %x tdr %x\n",
			(unsigned int)chan,
			(unsigned int)dma_read(chan, XILINX_DMA_STATUS_OFFSET),
			(unsigned int)dma_read(chan, XILINX_DMA_CDESC_OFFSET),
			(unsigned int)dma_read(chan, XILINX_DMA_TDESC_OFFSET));
		chan->err = 1;
	}


	/*
	 * Device takes too long to do the transfer when user requires
	 * responsiveness
	 */
	if (stat & XILINX_DMA_XR_IRQ_DELAY_MASK)
		dev_dbg(chan->dev, "Inter-packet latency too long\n");

	if (stat & XILINX_DMA_XR_IRQ_IOC_MASK) 
		tasklet_schedule(&chan->tasklet);

	/* Enable interrupts */
	dma_write(chan, XILINX_DMA_CONTROL_OFFSET,
			reg | XILINX_DMA_XR_IRQ_ALL_MASK);

	return IRQ_HANDLED;
}


static struct xilinx_dma_desc_sw *xilinx_dma_alloc_descriptor(struct xilinx_dma_chan *chan, dma_addr_t *dma_addr) {
	struct xilinx_dma_desc_sw *desc;

	desc = dma_pool_alloc(chan->desc_pool, GFP_ATOMIC, dma_addr);
	if (!desc) {
		dev_dbg(chan->dev, "out of memory for desc\n");
		return NULL;
	}

	memset(desc, 0, sizeof(*desc));
	return desc;
}

static void xilinx_dma_free_channels(struct xilinx_dma_device *xdev)
{
	int i;

	for (i = 0; i < XILINX_DMA_MAX_CHANS_PER_DEVICE; i++) {
		list_del(&xdev->chan[i]->common.device_node);
		tasklet_kill(&xdev->chan[i]->tasklet);
		irq_dispose_mapping(xdev->chan[i]->irq);
	}
}

/*
 * Probing channels
 *
 * . Get channel features from the device tree entry
 * . Initialize special channel handling routines
 */
static int xilinx_dma_chan_probe(struct xilinx_dma_device *xdev,
				 struct device_node *node, u32 feature)
{
	int i,err;
	u32 device_id, value, width = 0;
	struct xilinx_dma_chan *chan;
	struct xilinx_dma_desc_sw *new = NULL;
	struct xilinx_dma_desc_sw *prev = NULL;
	struct xilinx_dma_desc_sw *first = NULL;
	dma_addr_t dma_addr;

	/* alloc channel */
	chan = devm_kzalloc(xdev->dev, sizeof(*chan), GFP_KERNEL);
	if (!chan)
		return -ENOMEM;

	chan->feature = feature;
	chan->max_len = XILINX_DMA_MAX_TRANS_LEN;

	chan->has_dre = of_property_read_bool(node, "xlnx,include-dre");

	err = of_property_read_u32(node, "xlnx,datawidth", &value);
	if (err) {
		dev_err(xdev->dev, "unable to read datawidth property");
		return err;
	} else {
		width = value >> 3; /* convert bits to bytes */

		/* If data width is greater than 8 bytes, DRE is not in hw */
		if (width > 8)
			chan->has_dre = 0;

		chan->feature |= width - 1;
	}

	err = of_property_read_u32(node, "xlnx,device-id", &device_id);
	if (err) {
		dev_err(xdev->dev, "unable to read device id property");
		return err;
	}

	chan->has_sg = (xdev->feature & XILINX_DMA_FTR_HAS_SG) >>
		       XILINX_DMA_FTR_HAS_SG_SHIFT;

	//chan->start_transfer = xilinx_dma_start_transfer;

	if (of_device_is_compatible(node, "xlnx,axi-dma-mm2s-channel"))
		chan->direction = DMA_MEM_TO_DEV;

	if (of_device_is_compatible(node, "xlnx,axi-dma-s2mm-channel"))
		chan->direction = DMA_DEV_TO_MEM;

	chan->regs = xdev->regs;

	if (chan->direction == DMA_DEV_TO_MEM) {
		chan->regs = (xdev->regs + XILINX_DMA_RX_CHANNEL_OFFSET);
		chan->id = 1;
	}

	/*
	 * Used by dmatest channel matching in slave transfers
	 * Can change it to be a structure to have more matching information
	 */
	chan->private = (chan->direction & 0xFF) | XILINX_DMA_IP_DMA |
			(device_id << XILINX_DMA_DEVICE_ID_SHIFT);
	chan->common.private = (void *)&(chan->private);

	if (!chan->has_dre)
		xdev->common.copy_align = fls(width - 1);

	chan->dev = xdev->dev;
	xdev->chan[chan->id] = chan;

	/* Initialize the channel */
	err = dma_reset(chan);
	if (err) {
		dev_err(xdev->dev, "Reset channel failed\n");
		return err;
	}

	/*
	 * We need the descriptor to be aligned to 64bytes
	 * for meeting Xilinx DMA specification requirement.
	 */
	chan->desc_pool = dma_pool_create("xilinx_dma_desc_pool", chan->dev,
				sizeof(struct xilinx_dma_desc_sw), __alignof__(struct xilinx_dma_desc_sw), 0);
	if (!chan->desc_pool) {
		dev_err(chan->dev, "unable to allocate channel %d descriptor pool\n", chan->id);
		return -ENOMEM;
	}

	spin_lock_init(&chan->lock);
	INIT_LIST_HEAD(&chan->desc_list);

	// create loop of descriptors
	for (i=0;i<DESC_CNT;i++) {

			prev = new;
			/* Allocate the link descriptor from DMA pool */
			new = xilinx_dma_alloc_descriptor(chan, &dma_addr);
			if (!new) {
				dev_err(chan->dev,
					"No free memory for link descriptor\n");
				goto error;
			}
			new->phys = dma_addr;

			if (i==0) 
				first = new;
			else {
				prev->hw.next_desc = (u32)new->phys;
				prev->next = new;
			}

			new->buf = dma_alloc_coherent(chan->dev, RADIUM_DMA_BUF_SIZE, &new->dma_buf, GFP_KERNEL); 
			if (!new->buf) {
				printk(KERN_ERR "radium: error dma_alloc");
				goto error;
			}

			new->hw.buf_addr = new->dma_buf;
			new->hw.control = RADIUM_DMA_BUF_SIZE | XILINX_DMA_BD_SOP | XILINX_DMA_BD_EOP; 
			new->hw.status = XILINX_DMA_SR_CMP_MASK;

			memset(new->buf, 0, RADIUM_DMA_BUF_SIZE);
	}
	pr_debug("Probing xilinx axi dma direction %d\n",chan->direction);

	// loop last to first descriptors
	new->hw.next_desc = (u32)first->phys;
	new->next = first;
	chan->desc = first;

	chan->common.device = &xdev->common;

	/* find the IRQ line, if it exists in the device tree */
	chan->irq = irq_of_parse_and_map(node, 0);
	err = devm_request_irq(xdev->dev, chan->irq, dma_intr_handler,
			       IRQF_SHARED, "xilinx-dma-controller", chan);
	if (err) {
		dev_err(xdev->dev, "unable to request IRQ\n");
		return err;
	}

	tasklet_init(&chan->tasklet, dma_do_tasklet, (unsigned long)chan);

	/* Add the channel to DMA device channel list */
	list_add_tail(&chan->common.device_node, &xdev->common.channels);

	dma_reset(chan);
error:
	return 0;
}

static int radium_dma_probe(struct platform_device *pdev)
{
	struct device_node *child, *node;
	struct resource *res;
	int ret;
	u32 value;

	xdev = devm_kzalloc(&pdev->dev, sizeof(*xdev), GFP_KERNEL);
	if (!xdev)
		return -ENOMEM;

	xdev->dev = &(pdev->dev);
	INIT_LIST_HEAD(&xdev->common.channels);

	node = pdev->dev.of_node;

	/* iomap registers */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	xdev->regs = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(xdev->regs))
		return PTR_ERR(xdev->regs);

	/* Check if SG is enabled */
	value = of_property_read_bool(node, "xlnx,include-sg");
	if (value)
		xdev->feature |= XILINX_DMA_FTR_HAS_SG;

	/* Check if status control streams are enabled */
	value = of_property_read_bool(node,
				      "xlnx,sg-include-stscntrl-strm");
	if (value)
		xdev->feature |= XILINX_DMA_FTR_STSCNTRL_STRM;

	/* Axi DMA only do slave transfers */
	dma_cap_set(DMA_SLAVE, xdev->common.cap_mask);
	dma_cap_set(DMA_PRIVATE, xdev->common.cap_mask);

	platform_set_drvdata(pdev, xdev);

	for_each_child_of_node(node, child) {
		ret = xilinx_dma_chan_probe(xdev, child, xdev->feature);
		if (ret) {
			dev_err(&pdev->dev, "Probing channels failed\n");
			goto free_chan_resources;
		}
	}

	ret = dma_async_device_register(&xdev->common);
	if (ret) {
		dev_err(&pdev->dev, "DMA device registration failed\n");
		goto free_chan_resources;
	}

	rad_prepare_rx();
	dev_info(&pdev->dev, "Probing xilinx axi dma engine...Successful\n");

	return 0;

free_chan_resources:
	xilinx_dma_free_channels(xdev);
	return ret;
}

static int radium_dma_remove(struct platform_device *pdev)
{
	struct xilinx_dma_device *xdev;

	xdev = platform_get_drvdata(pdev);
	dma_async_device_unregister(&xdev->common);

	xilinx_dma_free_channels(xdev);


	return 0;
}
//------------------------------------------------------------------------------

static const struct of_device_id radium_dma_of_match[] = {
	{ .compatible = "xlnx,axi-dma", },
	{}
};
MODULE_DEVICE_TABLE(of, radium_dma_of_match);

static struct platform_driver radium_dma_driver = {
	.driver = {
		.name = "xilinx-dma",
		.of_match_table = radium_dma_of_match,
	},
	.probe = radium_dma_probe,
	.remove = radium_dma_remove,
};

//------------------------------------------------------------------------------

static const struct net_device_ops rad_netdev_ops = {
	.ndo_open               = rad_open,
	.ndo_start_xmit         = rad_xmit,
	.ndo_stop               = rad_stop,
	.ndo_set_mac_address    = eth_mac_addr
};

void rad_setup(struct net_device *net_dev)
{
	dev_dbg(&net_dev->dev, "%s\n", __func__);

	ether_setup(net_dev);
	net_dev->netdev_ops = &rad_netdev_ops;

	memcpy(net_dev->dev_addr, MAC_ADDR, ETH_ALEN);
}


static int __init rad_init(void) {
	struct netdev_rad_private *netdev_rad_private;
	int err=0;

	pr_debug("radium init\n");

	err =  platform_driver_register(&radium_dma_driver);
	if (err) {
		printk(KERN_ERR "Error register radium dma platform driver\n");
		return err;
	}

	/* Init netdev */
	netdev_rad = alloc_netdev(sizeof(struct netdev_rad_private), RAD_NETDEV_NAME, NET_NAME_UNKNOWN, rad_setup);
	if(netdev_rad == NULL) {
		printk(KERN_ERR "Error allocating Radium net device\n");
		return -ENOMEM;
	}

	if((err = register_netdev(netdev_rad))) {
		printk(KERN_ERR "Error %d initializing Radium net device\n", err);
		goto free_netdev1;
	}
	netdev_rad_private = netdev_priv(netdev_rad);
	memset(netdev_rad_private, 0, sizeof(struct netdev_rad_private));
	netdev_rad_private->net_device = netdev_rad;

	return err;

// free_netdev_unreg:
	unregister_netdev(netdev_rad);
free_netdev1:
	free_netdev(netdev_rad);

	return err;
}

static void __exit rad_clean(void)
{
	unregister_netdev(netdev_rad);
	free_netdev(netdev_rad);
	platform_driver_unregister(&radium_dma_driver);
}


//------------------------------------------------------------------------------

module_init (rad_init);
module_exit (rad_clean);

MODULE_AUTHOR("DL2STG");
MODULE_DESCRIPTION("R2T2 fpga dma network driver");
MODULE_LICENSE("GPL");
