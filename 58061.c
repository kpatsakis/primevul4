static netdev_tx_t catc_start_xmit(struct sk_buff *skb,
					 struct net_device *netdev)
{
	struct catc *catc = netdev_priv(netdev);
	unsigned long flags;
	int r = 0;
	char *tx_buf;

	spin_lock_irqsave(&catc->tx_lock, flags);

	catc->tx_ptr = (((catc->tx_ptr - 1) >> 6) + 1) << 6;
	tx_buf = catc->tx_buf[catc->tx_idx] + catc->tx_ptr;
	if (catc->is_f5u011)
		*(__be16 *)tx_buf = cpu_to_be16(skb->len);
	else
		*(__le16 *)tx_buf = cpu_to_le16(skb->len);
	skb_copy_from_linear_data(skb, tx_buf + 2, skb->len);
	catc->tx_ptr += skb->len + 2;

	if (!test_and_set_bit(TX_RUNNING, &catc->flags)) {
		r = catc_tx_run(catc);
		if (r < 0)
			clear_bit(TX_RUNNING, &catc->flags);
	}

	if ((catc->is_f5u011 && catc->tx_ptr) ||
	    (catc->tx_ptr >= ((TX_MAX_BURST - 1) * (PKT_SZ + 2))))
		netif_stop_queue(netdev);

	spin_unlock_irqrestore(&catc->tx_lock, flags);

	if (r >= 0) {
		catc->netdev->stats.tx_bytes += skb->len;
		catc->netdev->stats.tx_packets++;
	}

	dev_kfree_skb(skb);

	return NETDEV_TX_OK;
}
