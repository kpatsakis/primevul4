static void __tg3_set_coalesce(struct tg3 *tp, struct ethtool_coalesce *ec)
{
	tg3_coal_tx_init(tp, ec);
	tg3_coal_rx_init(tp, ec);

	if (!tg3_flag(tp, 5705_PLUS)) {
		u32 val = ec->stats_block_coalesce_usecs;

		tw32(HOSTCC_RXCOAL_TICK_INT, ec->rx_coalesce_usecs_irq);
		tw32(HOSTCC_TXCOAL_TICK_INT, ec->tx_coalesce_usecs_irq);

		if (!tp->link_up)
			val = 0;

		tw32(HOSTCC_STAT_COAL_TICKS, val);
	}
}
