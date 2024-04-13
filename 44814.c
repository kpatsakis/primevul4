static inline void wanxl_tx_intr(port_t *port)
{
	struct net_device *dev = port->dev;
	while (1) {
                desc_t *desc = &get_status(port)->tx_descs[port->tx_in];
		struct sk_buff *skb = port->tx_skbs[port->tx_in];

		switch (desc->stat) {
		case PACKET_FULL:
		case PACKET_EMPTY:
			netif_wake_queue(dev);
			return;

		case PACKET_UNDERRUN:
			dev->stats.tx_errors++;
			dev->stats.tx_fifo_errors++;
			break;

		default:
			dev->stats.tx_packets++;
			dev->stats.tx_bytes += skb->len;
		}
                desc->stat = PACKET_EMPTY; /* Free descriptor */
		pci_unmap_single(port->card->pdev, desc->address, skb->len,
				 PCI_DMA_TODEVICE);
		dev_kfree_skb_irq(skb);
                port->tx_in = (port->tx_in + 1) % TX_BUFFERS;
        }
}
