static netdev_tx_t tun_net_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct tun_struct *tun = netdev_priv(dev);

	tun_debug(KERN_INFO, tun, "tun_net_xmit %d\n", skb->len);

	/* Drop packet if interface is not attached */
	if (!tun->tfile)
		goto drop;

	/* Drop if the filter does not like it.
	 * This is a noop if the filter is disabled.
	 * Filter can be enabled only for the TAP devices. */
	if (!check_filter(&tun->txflt, skb))
		goto drop;

	if (tun->socket.sk->sk_filter &&
	    sk_filter(tun->socket.sk, skb))
		goto drop;

	if (skb_queue_len(&tun->socket.sk->sk_receive_queue) >= dev->tx_queue_len) {
		if (!(tun->flags & TUN_ONE_QUEUE)) {
			/* Normal queueing mode. */
			/* Packet scheduler handles dropping of further packets. */
			netif_stop_queue(dev);

			/* We won't see all dropped packets individually, so overrun
			 * error is more appropriate. */
			dev->stats.tx_fifo_errors++;
		} else {
			/* Single queue mode.
			 * Driver handles dropping of all packets itself. */
			goto drop;
		}
	}

	/* Orphan the skb - required as we might hang on to it
	 * for indefinite time. */
	if (unlikely(skb_orphan_frags(skb, GFP_ATOMIC)))
		goto drop;
	skb_orphan(skb);

	/* Enqueue packet */
	skb_queue_tail(&tun->socket.sk->sk_receive_queue, skb);

	/* Notify and wake up reader process */
	if (tun->flags & TUN_FASYNC)
		kill_fasync(&tun->fasync, SIGIO, POLL_IN);
	wake_up_interruptible_poll(&tun->wq.wait, POLLIN |
				   POLLRDNORM | POLLRDBAND);
	return NETDEV_TX_OK;

drop:
	dev->stats.tx_dropped++;
	kfree_skb(skb);
	return NETDEV_TX_OK;
}
