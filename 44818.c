do_bottom_half_tx(struct fst_card_info *card)
{
	struct fst_port_info *port;
	int pi;
	int txq_length;
	struct sk_buff *skb;
	unsigned long flags;
	struct net_device *dev;

	/*
	 *  Find a free buffer for the transmit
	 *  Step through each port on this card
	 */

	dbg(DBG_TX, "do_bottom_half_tx\n");
	for (pi = 0, port = card->ports; pi < card->nports; pi++, port++) {
		if (!port->run)
			continue;

		dev = port_to_dev(port);
		while (!(FST_RDB(card, txDescrRing[pi][port->txpos].bits) &
			 DMA_OWN) &&
		       !(card->dmatx_in_progress)) {
			/*
			 * There doesn't seem to be a txdone event per-se
			 * We seem to have to deduce it, by checking the DMA_OWN
			 * bit on the next buffer we think we can use
			 */
			spin_lock_irqsave(&card->card_lock, flags);
			if ((txq_length = port->txqe - port->txqs) < 0) {
				/*
				 * This is the case where one has wrapped and the
				 * maths gives us a negative number
				 */
				txq_length = txq_length + FST_TXQ_DEPTH;
			}
			spin_unlock_irqrestore(&card->card_lock, flags);
			if (txq_length > 0) {
				/*
				 * There is something to send
				 */
				spin_lock_irqsave(&card->card_lock, flags);
				skb = port->txq[port->txqs];
				port->txqs++;
				if (port->txqs == FST_TXQ_DEPTH) {
					port->txqs = 0;
				}
				spin_unlock_irqrestore(&card->card_lock, flags);
				/*
				 * copy the data and set the required indicators on the
				 * card.
				 */
				FST_WRW(card, txDescrRing[pi][port->txpos].bcnt,
					cnv_bcnt(skb->len));
				if ((skb->len < FST_MIN_DMA_LEN) ||
				    (card->family == FST_FAMILY_TXP)) {
					/* Enqueue the packet with normal io */
					memcpy_toio(card->mem +
						    BUF_OFFSET(txBuffer[pi]
							       [port->
								txpos][0]),
						    skb->data, skb->len);
					FST_WRB(card,
						txDescrRing[pi][port->txpos].
						bits,
						DMA_OWN | TX_STP | TX_ENP);
					dev->stats.tx_packets++;
					dev->stats.tx_bytes += skb->len;
					dev->trans_start = jiffies;
				} else {
					/* Or do it through dma */
					memcpy(card->tx_dma_handle_host,
					       skb->data, skb->len);
					card->dma_port_tx = port;
					card->dma_len_tx = skb->len;
					card->dma_txpos = port->txpos;
					fst_tx_dma(card,
						   (char *) card->
						   tx_dma_handle_card,
						   (char *)
						   BUF_OFFSET(txBuffer[pi]
							      [port->txpos][0]),
						   skb->len);
				}
				if (++port->txpos >= NUM_TX_BUFFER)
					port->txpos = 0;
				/*
				 * If we have flow control on, can we now release it?
				 */
				if (port->start) {
					if (txq_length < fst_txq_low) {
						netif_wake_queue(port_to_dev
								 (port));
						port->start = 0;
					}
				}
				dev_kfree_skb(skb);
			} else {
				/*
				 * Nothing to send so break out of the while loop
				 */
				break;
			}
		}
	}
}
