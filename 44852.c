fst_rx_dma_complete(struct fst_card_info *card, struct fst_port_info *port,
		    int len, struct sk_buff *skb, int rxp)
{
	struct net_device *dev = port_to_dev(port);
	int pi;
	int rx_status;

	dbg(DBG_TX, "fst_rx_dma_complete\n");
	pi = port->index;
	memcpy(skb_put(skb, len), card->rx_dma_handle_host, len);

	/* Reset buffer descriptor */
	FST_WRB(card, rxDescrRing[pi][rxp].bits, DMA_OWN);

	/* Update stats */
	dev->stats.rx_packets++;
	dev->stats.rx_bytes += len;

	/* Push upstream */
	dbg(DBG_RX, "Pushing the frame up the stack\n");
	if (port->mode == FST_RAW)
		skb->protocol = farsync_type_trans(skb, dev);
	else
		skb->protocol = hdlc_type_trans(skb, dev);
	rx_status = netif_rx(skb);
	fst_process_rx_status(rx_status, port_to_dev(port)->name);
	if (rx_status == NET_RX_DROP)
		dev->stats.rx_dropped++;
}
