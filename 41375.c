static void tg3_init_coal(struct tg3 *tp)
{
	struct ethtool_coalesce *ec = &tp->coal;

	memset(ec, 0, sizeof(*ec));
	ec->cmd = ETHTOOL_GCOALESCE;
	ec->rx_coalesce_usecs = LOW_RXCOL_TICKS;
	ec->tx_coalesce_usecs = LOW_TXCOL_TICKS;
	ec->rx_max_coalesced_frames = LOW_RXMAX_FRAMES;
	ec->tx_max_coalesced_frames = LOW_TXMAX_FRAMES;
	ec->rx_coalesce_usecs_irq = DEFAULT_RXCOAL_TICK_INT;
	ec->tx_coalesce_usecs_irq = DEFAULT_TXCOAL_TICK_INT;
	ec->rx_max_coalesced_frames_irq = DEFAULT_RXCOAL_MAXF_INT;
	ec->tx_max_coalesced_frames_irq = DEFAULT_TXCOAL_MAXF_INT;
	ec->stats_block_coalesce_usecs = DEFAULT_STAT_COAL_TICKS;

	if (tp->coalesce_mode & (HOSTCC_MODE_CLRTICK_RXBD |
				 HOSTCC_MODE_CLRTICK_TXBD)) {
		ec->rx_coalesce_usecs = LOW_RXCOL_TICKS_CLRTCKS;
		ec->rx_coalesce_usecs_irq = DEFAULT_RXCOAL_TICK_INT_CLRTCKS;
		ec->tx_coalesce_usecs = LOW_TXCOL_TICKS_CLRTCKS;
		ec->tx_coalesce_usecs_irq = DEFAULT_TXCOAL_TICK_INT_CLRTCKS;
	}

	if (tg3_flag(tp, 5705_PLUS)) {
		ec->rx_coalesce_usecs_irq = 0;
		ec->tx_coalesce_usecs_irq = 0;
		ec->stats_block_coalesce_usecs = 0;
	}
}
