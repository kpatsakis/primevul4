static int wanxl_close(struct net_device *dev)
{
	port_t *port = dev_to_port(dev);
	unsigned long timeout;
	int i;

	hdlc_close(dev);
	/* signal the card */
	writel(1 << (DOORBELL_TO_CARD_CLOSE_0 + port->node),
	       port->card->plx + PLX_DOORBELL_TO_CARD);

	timeout = jiffies + HZ;
	do {
		if (!get_status(port)->open)
			break;
	} while (time_after(timeout, jiffies));

	if (get_status(port)->open)
		netdev_err(dev, "unable to close port\n");

	netif_stop_queue(dev);

	for (i = 0; i < TX_BUFFERS; i++) {
		desc_t *desc = &get_status(port)->tx_descs[i];

		if (desc->stat != PACKET_EMPTY) {
			desc->stat = PACKET_EMPTY;
			pci_unmap_single(port->card->pdev, desc->address,
					 port->tx_skbs[i]->len,
					 PCI_DMA_TODEVICE);
			dev_kfree_skb(port->tx_skbs[i]);
		}
	}
	return 0;
}
