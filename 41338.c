static void tg3_get_channels(struct net_device *dev,
			     struct ethtool_channels *channel)
{
	struct tg3 *tp = netdev_priv(dev);
	u32 deflt_qs = netif_get_num_default_rss_queues();

	channel->max_rx = tp->rxq_max;
	channel->max_tx = tp->txq_max;

	if (netif_running(dev)) {
		channel->rx_count = tp->rxq_cnt;
		channel->tx_count = tp->txq_cnt;
	} else {
		if (tp->rxq_req)
			channel->rx_count = tp->rxq_req;
		else
			channel->rx_count = min(deflt_qs, tp->rxq_max);

		if (tp->txq_req)
			channel->tx_count = tp->txq_req;
		else
			channel->tx_count = min(deflt_qs, tp->txq_max);
	}
}
