fst_close(struct net_device *dev)
{
	struct fst_port_info *port;
	struct fst_card_info *card;
	unsigned char tx_dma_done;
	unsigned char rx_dma_done;

	port = dev_to_port(dev);
	card = port->card;

	tx_dma_done = inb(card->pci_conf + DMACSR1);
	rx_dma_done = inb(card->pci_conf + DMACSR0);
	dbg(DBG_OPEN,
	    "Port Close: tx_dma_in_progress = %d (%x) rx_dma_in_progress = %d (%x)\n",
	    card->dmatx_in_progress, tx_dma_done, card->dmarx_in_progress,
	    rx_dma_done);

	netif_stop_queue(dev);
	fst_closeport(dev_to_port(dev));
	if (port->mode != FST_RAW) {
		hdlc_close(dev);
	}
	module_put(THIS_MODULE);
	return 0;
}
