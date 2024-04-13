static netdev_tx_t wanxl_xmit(struct sk_buff *skb, struct net_device *dev)
{
        port_t *port = dev_to_port(dev);
	desc_t *desc;

        spin_lock(&port->lock);

	desc = &get_status(port)->tx_descs[port->tx_out];
        if (desc->stat != PACKET_EMPTY) {
                /* should never happen - previous xmit should stop queue */
#ifdef DEBUG_PKT
                printk(KERN_DEBUG "%s: transmitter buffer full\n", dev->name);
#endif
		netif_stop_queue(dev);
		spin_unlock(&port->lock);
		return NETDEV_TX_BUSY;       /* request packet to be queued */
	}

#ifdef DEBUG_PKT
	printk(KERN_DEBUG "%s TX(%i):", dev->name, skb->len);
	debug_frame(skb);
#endif

	port->tx_skbs[port->tx_out] = skb;
	desc->address = pci_map_single(port->card->pdev, skb->data, skb->len,
				       PCI_DMA_TODEVICE);
	desc->length = skb->len;
	desc->stat = PACKET_FULL;
	writel(1 << (DOORBELL_TO_CARD_TX_0 + port->node),
	       port->card->plx + PLX_DOORBELL_TO_CARD);

	port->tx_out = (port->tx_out + 1) % TX_BUFFERS;

	if (get_status(port)->tx_descs[port->tx_out].stat != PACKET_EMPTY) {
		netif_stop_queue(dev);
#ifdef DEBUG_PKT
		printk(KERN_DEBUG "%s: transmitter buffer full\n", dev->name);
#endif
	}

	spin_unlock(&port->lock);
	return NETDEV_TX_OK;
}
