static inline void wanxl_rx_intr(card_t *card)
{
	desc_t *desc;
	while (desc = &card->status->rx_descs[card->rx_in],
	       desc->stat != PACKET_EMPTY) {
		if ((desc->stat & PACKET_PORT_MASK) > card->n_ports)
			pr_crit("%s: received packet for nonexistent port\n",
				pci_name(card->pdev));
		else {
			struct sk_buff *skb = card->rx_skbs[card->rx_in];
			port_t *port = &card->ports[desc->stat &
						    PACKET_PORT_MASK];
			struct net_device *dev = port->dev;

			if (!skb)
				dev->stats.rx_dropped++;
			else {
				pci_unmap_single(card->pdev, desc->address,
						 BUFFER_LENGTH,
						 PCI_DMA_FROMDEVICE);
				skb_put(skb, desc->length);

#ifdef DEBUG_PKT
				printk(KERN_DEBUG "%s RX(%i):", dev->name,
				       skb->len);
				debug_frame(skb);
#endif
				dev->stats.rx_packets++;
				dev->stats.rx_bytes += skb->len;
				skb->protocol = hdlc_type_trans(skb, dev);
				netif_rx(skb);
				skb = NULL;
			}

			if (!skb) {
				skb = dev_alloc_skb(BUFFER_LENGTH);
				desc->address = skb ?
					pci_map_single(card->pdev, skb->data,
						       BUFFER_LENGTH,
						       PCI_DMA_FROMDEVICE) : 0;
				card->rx_skbs[card->rx_in] = skb;
			}
		}
		desc->stat = PACKET_EMPTY; /* Free descriptor */
		card->rx_in = (card->rx_in + 1) % RX_QUEUE_LENGTH;
	}
}
