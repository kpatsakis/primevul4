static int tg3_set_coalesce(struct net_device *dev, struct ethtool_coalesce *ec)
{
	struct tg3 *tp = netdev_priv(dev);
	u32 max_rxcoal_tick_int = 0, max_txcoal_tick_int = 0;
	u32 max_stat_coal_ticks = 0, min_stat_coal_ticks = 0;

	if (!tg3_flag(tp, 5705_PLUS)) {
		max_rxcoal_tick_int = MAX_RXCOAL_TICK_INT;
		max_txcoal_tick_int = MAX_TXCOAL_TICK_INT;
		max_stat_coal_ticks = MAX_STAT_COAL_TICKS;
		min_stat_coal_ticks = MIN_STAT_COAL_TICKS;
	}

	if ((ec->rx_coalesce_usecs > MAX_RXCOL_TICKS) ||
	    (ec->tx_coalesce_usecs > MAX_TXCOL_TICKS) ||
	    (ec->rx_max_coalesced_frames > MAX_RXMAX_FRAMES) ||
	    (ec->tx_max_coalesced_frames > MAX_TXMAX_FRAMES) ||
	    (ec->rx_coalesce_usecs_irq > max_rxcoal_tick_int) ||
	    (ec->tx_coalesce_usecs_irq > max_txcoal_tick_int) ||
	    (ec->rx_max_coalesced_frames_irq > MAX_RXCOAL_MAXF_INT) ||
	    (ec->tx_max_coalesced_frames_irq > MAX_TXCOAL_MAXF_INT) ||
	    (ec->stats_block_coalesce_usecs > max_stat_coal_ticks) ||
	    (ec->stats_block_coalesce_usecs < min_stat_coal_ticks))
		return -EINVAL;

	/* No rx interrupts will be generated if both are zero */
	if ((ec->rx_coalesce_usecs == 0) &&
	    (ec->rx_max_coalesced_frames == 0))
		return -EINVAL;

	/* No tx interrupts will be generated if both are zero */
	if ((ec->tx_coalesce_usecs == 0) &&
	    (ec->tx_max_coalesced_frames == 0))
		return -EINVAL;

	/* Only copy relevant parameters, ignore all others. */
	tp->coal.rx_coalesce_usecs = ec->rx_coalesce_usecs;
	tp->coal.tx_coalesce_usecs = ec->tx_coalesce_usecs;
	tp->coal.rx_max_coalesced_frames = ec->rx_max_coalesced_frames;
	tp->coal.tx_max_coalesced_frames = ec->tx_max_coalesced_frames;
	tp->coal.rx_coalesce_usecs_irq = ec->rx_coalesce_usecs_irq;
	tp->coal.tx_coalesce_usecs_irq = ec->tx_coalesce_usecs_irq;
	tp->coal.rx_max_coalesced_frames_irq = ec->rx_max_coalesced_frames_irq;
	tp->coal.tx_max_coalesced_frames_irq = ec->tx_max_coalesced_frames_irq;
	tp->coal.stats_block_coalesce_usecs = ec->stats_block_coalesce_usecs;

	if (netif_running(dev)) {
		tg3_full_lock(tp, 0);
		__tg3_set_coalesce(tp, &tp->coal);
		tg3_full_unlock(tp);
	}
	return 0;
}
