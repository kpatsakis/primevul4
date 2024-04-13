static void tg3_get_ringparam(struct net_device *dev, struct ethtool_ringparam *ering)
{
	struct tg3 *tp = netdev_priv(dev);

	ering->rx_max_pending = tp->rx_std_ring_mask;
	if (tg3_flag(tp, JUMBO_RING_ENABLE))
		ering->rx_jumbo_max_pending = tp->rx_jmb_ring_mask;
	else
		ering->rx_jumbo_max_pending = 0;

	ering->tx_max_pending = TG3_TX_RING_SIZE - 1;

	ering->rx_pending = tp->rx_pending;
	if (tg3_flag(tp, JUMBO_RING_ENABLE))
		ering->rx_jumbo_pending = tp->rx_jumbo_pending;
	else
		ering->rx_jumbo_pending = 0;

	ering->tx_pending = tp->napi[0].tx_pending;
}
