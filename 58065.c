static void catc_tx_done(struct urb *urb)
{
	struct catc *catc = urb->context;
	unsigned long flags;
	int r, status = urb->status;

	if (status == -ECONNRESET) {
		dev_dbg(&urb->dev->dev, "Tx Reset.\n");
		urb->status = 0;
		netif_trans_update(catc->netdev);
		catc->netdev->stats.tx_errors++;
		clear_bit(TX_RUNNING, &catc->flags);
		netif_wake_queue(catc->netdev);
		return;
	}

	if (status) {
		dev_dbg(&urb->dev->dev, "tx_done, status %d, length %d\n",
			status, urb->actual_length);
		return;
	}

	spin_lock_irqsave(&catc->tx_lock, flags);

	if (catc->tx_ptr) {
		r = catc_tx_run(catc);
		if (unlikely(r < 0))
			clear_bit(TX_RUNNING, &catc->flags);
	} else {
		clear_bit(TX_RUNNING, &catc->flags);
	}

	netif_wake_queue(catc->netdev);

	spin_unlock_irqrestore(&catc->tx_lock, flags);
}
