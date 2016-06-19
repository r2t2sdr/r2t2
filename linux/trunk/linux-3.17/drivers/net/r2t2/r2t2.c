/*
 * Copyright (C) 2011 NetModule AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
//#define DEBUG

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/rpmsg.h>
#include <linux/interrupt.h>
#include <linux/circ_buf.h>

#define WDL_NETDEV_NAME "r2t2%d"
#define RPMSG_BUFF_SIZE 512
#define WDL_TX_RING_BUFFER_SIZE 1 //128
#define WDL_RX_RING_BUFFER_SIZE 1 //128

/* The instantiated net device */
static struct net_device* netdev_r2t2;

/* The ring buffer
 * FIXME: Can be used by different CPU, stupid cache ... volatile, wmb, ... ???
 * However, in our AMP configuration it is probably not the case as only one CPU
 * is handling Linux, we just should not forget about this!
 */
struct r2t2_ring_buffer
{
  unsigned int head;
  unsigned int tail;
  unsigned int ring_size;
  unsigned int count;
  spinlock_t lock;
  struct sk_buff **skbs;
};

/* r2t2 net device private data */
struct netdev_r2t2_private{
  struct net_device *net_device;
  struct rpmsg_channel *rpmsg_channel;
  struct work_struct r2t2_tx_w;
  struct r2t2_ring_buffer tx_buffer;
};

/* Tasklets */
static void r2t2_tx_tasklet_cb(unsigned long unused);
DECLARE_TASKLET(r2t2_tx_tasklet, r2t2_tx_tasklet_cb, 0);

/* R2T2 rpmessage types */
typedef enum {
    SET_VIRTIO_ADD = 0, // Set Linux virtio address
    CLEAR_VIRTIO_ADD,   // Clear Linux virtio address
    NEW_DATAGRAM        // New datagram data
} r2t2_rpmsg_types;

//------------------------------------------------------------------------------
int r2t2_init_ring(struct r2t2_ring_buffer *rb, unsigned int size)
{
  unsigned int i;

  rb->skbs = kmalloc(size * sizeof(void*), GFP_KERNEL);
  if(rb->skbs == NULL)
    return -ENOMEM;

  rb->head = 0;
  rb->tail = 0;
  rb->count = 0;
  rb->ring_size = size;
  spin_lock_init(&rb->lock);

  for(i = 0 ; i < rb->ring_size ; i++)
    rb->skbs[i] = NULL;

  return 0;
}

//------------------------------------------------------------------------------
int r2t2_is_ring_full(struct r2t2_ring_buffer *rb)
{
  int is_full;

  spin_lock(&rb->lock);
  is_full = rb->count >= rb->ring_size;
  spin_unlock(&rb->lock);

  return is_full;
}

//------------------------------------------------------------------------------
int r2t2_is_ring_empty(struct r2t2_ring_buffer *rb)
{
  int is_empty;

  spin_lock(&rb->lock);
  is_empty = rb->count == 0;
  spin_unlock(&rb->lock);

  return is_empty;
}

//------------------------------------------------------------------------------
void r2t2_add_skb_to_ring(struct r2t2_ring_buffer *rb, struct sk_buff *skb)
{
  /* Paranoid */
  if(unlikely(skb == NULL))
  {
    dev_err(&netdev_r2t2->dev, "Not adding an empty skb in ring buffer!\n");
    return;
  }

  spin_lock(&rb->lock);
  rb->skbs[rb->head] = skb;
  rb->head = (rb->head + 1) % rb->ring_size;
  rb->count++;
  spin_unlock(&rb->lock);
}

//------------------------------------------------------------------------------
struct sk_buff *r2t2_get_skb_from_ring(struct r2t2_ring_buffer *rb)
{
  struct sk_buff *skb;

  spin_lock(&rb->lock);
  skb = rb->skbs[rb->tail];
  rb->skbs[rb->tail] = NULL;
  rb->tail = (rb->tail + 1) % rb->ring_size;
  rb->count--;
  spin_unlock(&rb->lock);

  return skb;
}

//------------------------------------------------------------------------------
void r2t2_clean_ring(struct r2t2_ring_buffer *rb)
{
  unsigned int i;

  for(i = 0 ; i < rb->ring_size ; i++)
  {
    if(rb->skbs[i] != NULL)
      dev_kfree_skb(rb->skbs[i]);
  }
  kfree(rb->skbs);
}

//------------------------------------------------------------------------------
static int r2t2_open(struct net_device *net_dev)
{
  struct netdev_r2t2_private* priv = netdev_priv(net_dev);
  int err;

  dev_dbg(&net_dev->dev, "%s\n", __func__);

  /* Initialize ring buffer */
  err = r2t2_init_ring(&priv->tx_buffer, WDL_TX_RING_BUFFER_SIZE);
  if(err)
  {
    dev_err(&net_dev->dev, "Couldn't allocate TX buffer ring\n");
    return err;
  }

  netif_start_queue(net_dev);

  return 0;
}

//------------------------------------------------------------------------------
static void r2t2_tx_tasklet_cb(unsigned long unused)
{
  int err;
  r2t2_rpmsg_types msg_type = NEW_DATAGRAM;
  unsigned char *rpmsg_data;
  struct netdev_r2t2_private *r2t2_priv = netdev_priv(netdev_r2t2);
  struct r2t2_ring_buffer *rb = &r2t2_priv->tx_buffer;
  struct sk_buff *skb;

  dev_dbg(&r2t2_priv->net_device->dev, "%s ENTER %i entries in TX ring!\n",
      __func__, rb->count);

  /* Empty the ring buffer */
  while(!r2t2_is_ring_empty(rb))
  {
    dev_dbg(&r2t2_priv->net_device->dev, "%s data TX ring!\n", __func__);
    skb = r2t2_get_skb_from_ring(rb);

    /* Paranoid */
    if(unlikely(skb == NULL))
    {
      dev_err(&r2t2_priv->net_device->dev, "Empty SKB in ring buffer!\n");
      continue;
    }

    /* Make sure the skb fits in rpmessage */
    if(sizeof(r2t2_rpmsg_types) + skb->len > RPMSG_BUFF_SIZE)
    {
      /* FIXME: Handle this correctly, more than one rpmessage? */
      dev_err(&r2t2_priv->net_device->dev, "Too much skb data!\n");
      dev_kfree_skb(skb);
      continue;
    }

    /* Prepare the rpmsg, we must prepend the message type
     * FIXME: Use dedicated virtio or shared memory area for zero
     * copy/allocation ...
     */
    rpmsg_data = kmalloc(sizeof(r2t2_rpmsg_types) + skb->len, GFP_KERNEL);
    if(rpmsg_data == NULL)
    {
      dev_err(&r2t2_priv->net_device->dev, "Cannot allocate rpmsg data\n");
      dev_kfree_skb(skb);
      kfree(rpmsg_data);
      continue;
    }
    memcpy(rpmsg_data, &msg_type, sizeof(msg_type));
    memcpy(rpmsg_data + sizeof(msg_type), skb->data, skb->len);
    dev_kfree_skb(skb);

    /* Send skb data to FreeRTOS using rpmsg */
    err = rpmsg_trysend(r2t2_priv->rpmsg_channel, rpmsg_data,
        sizeof(r2t2_rpmsg_types) + skb->len);
    if(err)
        dev_err(&r2t2_priv->net_device->dev,
            "Cannot send rpmessage datagram: %d\n", err);

    kfree(rpmsg_data);
  }

  if(unlikely(netif_queue_stopped(r2t2_priv->net_device)))
    netif_wake_queue(r2t2_priv->net_device);

  dev_dbg(&r2t2_priv->net_device->dev, "%s EXIT!\n", __func__);
}

//------------------------------------------------------------------------------
static int r2t2_xmit(struct sk_buff *skb, struct net_device *net_dev)
{
  struct netdev_r2t2_private* priv = netdev_priv(net_dev);
  int ret = NETDEV_TX_OK;

  dev_dbg(&net_dev->dev, "%s\n", __func__);

  /* XRUN */
  if(r2t2_is_ring_full(&priv->tx_buffer))
  {
    dev_warn(&net_dev->dev, "TX Ring buffer is full, skb dropped\n");
    dev_kfree_skb(skb);
    netif_stop_queue(net_dev);
    ret = NETDEV_TX_BUSY;
  }
  else
  {
    r2t2_add_skb_to_ring(&priv->tx_buffer, skb);
    /* Send the skb in a tasklet */
    tasklet_hi_schedule(&r2t2_tx_tasklet);
  }

  return ret;
}

//------------------------------------------------------------------------------
static int r2t2_stop(struct net_device *net_dev)
{
  struct netdev_r2t2_private* priv = netdev_priv(net_dev);

  dev_dbg(&net_dev->dev, "%s\n", __func__);

  netif_stop_queue(net_dev);
  r2t2_clean_ring(&priv->tx_buffer);

  return 0;
}

//------------------------------------------------------------------------------
static const struct net_device_ops r2t2_netdev_ops = {
    .ndo_open               = r2t2_open,
    .ndo_start_xmit         = r2t2_xmit,
    .ndo_stop               = r2t2_stop,
    .ndo_set_mac_address    = eth_mac_addr
};

void r2t2_setup(struct net_device *net_dev)
{
  dev_dbg(&net_dev->dev, "%s\n", __func__);

  ether_setup(net_dev);
  net_dev->netdev_ops = &r2t2_netdev_ops;

  /* Set a dummy MAC address
   * FIXME : command line args, register, eeprom
   */
  memcpy(net_dev->dev_addr, "\0WDLDR", ETH_ALEN);
}

//------------------------------------------------------------------------------
static int rpmsg_r2t2_probe(struct rpmsg_channel *rp_dev)
{
  struct netdev_r2t2_private* priv = netdev_priv(netdev_r2t2);
  unsigned char rpmsg_data[sizeof(r2t2_rpmsg_types) + sizeof(rp_dev->src)];
  r2t2_rpmsg_types msg_type = SET_VIRTIO_ADD;
  int err;

  dev_dbg(&rp_dev->dev, "%s\n", __func__);

  /* Set the virtio channel to the net device */
  priv->rpmsg_channel = rp_dev;

  /* Send the destination virtio address to FreeRTOS */
  memcpy(rpmsg_data, &msg_type, sizeof(msg_type));
  memcpy(rpmsg_data + sizeof(msg_type), &rp_dev->src, sizeof(rp_dev->src));
  err = rpmsg_send(rp_dev, rpmsg_data, sizeof(msg_type) + sizeof(rp_dev->src));
  if(err)
  {
    dev_err(&rp_dev->dev, "Cannot send virtio address rpmessage\n");
    return err;
  }

  return 0;
}

//------------------------------------------------------------------------------
static void rpmsg_r2t2_remove(struct rpmsg_channel *rp_dev)
{
  r2t2_rpmsg_types msg_type = CLEAR_VIRTIO_ADD;
  int err;

  dev_dbg(&rp_dev->dev, "%s\n", __func__);

  /* Clear the destination virtio address in FreeRTOS */
  err = rpmsg_send_offchannel(rp_dev, rp_dev->src, rp_dev->dst, &msg_type,
      sizeof(msg_type));
  if(err)
    dev_err(&rp_dev->dev, "Cannot send clear virtio address rpmessage\n");
}

//------------------------------------------------------------------------------
static void rpmsg_r2t2_cb(struct rpmsg_channel *rp_dev, void *data,
    int len, void *priv, u32 src)
{
  struct sk_buff *skb;

  dev_dbg(&rp_dev->dev, "%s\n", __func__);

  skb = dev_alloc_skb(len + NET_IP_ALIGN);
  if(skb == NULL)
  {
    dev_err(&rp_dev->dev, "Cannot allocate new skb\n");
    return;
  }
  memcpy(skb_put(skb, len), data, len);

  skb->dev = netdev_r2t2;
  skb->protocol = eth_type_trans(skb, netdev_r2t2);

  /* Send the skb in a workqueue, we are in an interrupt ... */
  netif_rx_ni(skb);
}

//------------------------------------------------------------------------------
static struct rpmsg_device_id rpmsg_r2t2_id_table[] = {
    { .name = "rpmsg-r2t2" },
    { },
};

MODULE_DEVICE_TABLE(rpmsg, rpmsg_r2t2_id_table);

static struct rpmsg_driver rpmsg_r2t2_driver = {
    .drv.name   = KBUILD_MODNAME,
    .drv.owner  = THIS_MODULE,
    .id_table   = rpmsg_r2t2_id_table,
    .probe      = rpmsg_r2t2_probe,
    .callback   = rpmsg_r2t2_cb,
    .remove     = rpmsg_r2t2_remove,
};

//------------------------------------------------------------------------------
static int __init r2t2_init(void)
{
  struct netdev_r2t2_private *netdev_r2t2_private;
  int err;

  /* Init netdev */
  netdev_r2t2 = alloc_netdev(
      sizeof(struct netdev_r2t2_private), WDL_NETDEV_NAME, NET_NAME_UNKNOWN, r2t2_setup);
  if(netdev_r2t2 == NULL)
  {
    printk(KERN_ERR "Error allocating R2T2 net device\n");
    return -ENOMEM;
  }

  if((err = register_netdev(netdev_r2t2))) {
    printk(KERN_ERR "Error %d initializing R2T2 net device\n", err);
    goto free_netdev;
  }
  netdev_r2t2_private = netdev_priv(netdev_r2t2);
  memset(netdev_r2t2_private, 0, sizeof(struct netdev_r2t2_private));
  netdev_r2t2_private->net_device = netdev_r2t2;

  /* Init rpmsg */
  err = register_rpmsg_driver(&rpmsg_r2t2_driver);
  if(err)
  {
    printk(KERN_ERR "Cannot register R2T2 rpmsg driver\n");
    goto free_r2t2_rpmsg;
  }

  return 0;

free_r2t2_rpmsg:
  unregister_rpmsg_driver(&rpmsg_r2t2_driver);
  unregister_netdev(netdev_r2t2);
free_netdev:
  free_netdev(netdev_r2t2);

  return err;
}

//------------------------------------------------------------------------------
static void __exit r2t2_clean(void)
{
  unregister_netdev(netdev_r2t2);
  free_netdev(netdev_r2t2);
  unregister_rpmsg_driver(&rpmsg_r2t2_driver);
}

module_init (r2t2_init);
module_exit (r2t2_clean);

MODULE_AUTHOR("Netmodule AG");
MODULE_DESCRIPTION("R2T2 network driver");
MODULE_LICENSE("GPL");
