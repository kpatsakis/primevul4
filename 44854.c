fst_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct fst_card_info *card;
	struct fst_port_info *port;
	unsigned long flags;
	int txq_length;

	port = dev_to_port(dev);
	card = port->card;
	dbg(DBG_TX, "fst_start_xmit: length = %d\n", skb->len);

	/* Drop packet with error if we don't have carrier */
	if (!netif_carrier_ok(dev)) {
		dev_kfree_skb(skb);
		dev->stats.tx_errors++;
		dev->stats.tx_carrier_errors++;
		dbg(DBG_ASS,
		    "Tried to transmit but no carrier on card %d port %d\n",
		    card->card_no, port->index);
		return NETDEV_TX_OK;
	}

	/* Drop it if it's too big! MTU failure ? */
	if (skb->len > LEN_TX_BUFFER) {
		dbg(DBG_ASS, "Packet too large %d vs %d\n", skb->len,
		    LEN_TX_BUFFER);
		dev_kfree_skb(skb);
		dev->stats.tx_errors++;
		return NETDEV_TX_OK;
	}

	/*
	 * We are always going to queue the packet
	 * so that the bottom half is the only place we tx from
	 * Check there is room in the port txq
	 */
	spin_lock_irqsave(&card->card_lock, flags);
	if ((txq_length = port->txqe - port->txqs) < 0) {
		/*
		 * This is the case where the next free has wrapped but the
		 * last used hasn't
		 */
		txq_length = txq_length + FST_TXQ_DEPTH;
	}
	spin_unlock_irqrestore(&card->card_lock, flags);
	if (txq_length > fst_txq_high) {
		/*
		 * We have got enough buffers in the pipeline.  Ask the network
		 * layer to stop sending frames down
		 */
		netif_stop_queue(dev);
		port->start = 1;	/* I'm using this to signal stop sent up */
	}

	if (txq_length == FST_TXQ_DEPTH - 1) {
		/*
		 * This shouldn't have happened but such is life
		 */
		dev_kfree_skb(skb);
		dev->stats.tx_errors++;
		dbg(DBG_ASS, "Tx queue overflow card %d port %d\n",
		    card->card_no, port->index);
		return NETDEV_TX_OK;
	}

	/*
	 * queue the buffer
	 */
	spin_lock_irqsave(&card->card_lock, flags);
	port->txq[port->txqe] = skb;
	port->txqe++;
	if (port->txqe == FST_TXQ_DEPTH)
		port->txqe = 0;
	spin_unlock_irqrestore(&card->card_lock, flags);

	/* Scehdule the bottom half which now does transmit processing */
	fst_q_work_item(&fst_work_txq, card->card_no);
	tasklet_schedule(&fst_tx_task);

	return NETDEV_TX_OK;
}
