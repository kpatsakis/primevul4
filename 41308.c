static void tg3_coal_tx_init(struct tg3 *tp, struct ethtool_coalesce *ec)
{
	int i = 0;

	if (!tg3_flag(tp, ENABLE_TSS)) {
		tw32(HOSTCC_TXCOL_TICKS, ec->tx_coalesce_usecs);
		tw32(HOSTCC_TXMAX_FRAMES, ec->tx_max_coalesced_frames);
		tw32(HOSTCC_TXCOAL_MAXF_INT, ec->tx_max_coalesced_frames_irq);
	} else {
		tw32(HOSTCC_TXCOL_TICKS, 0);
		tw32(HOSTCC_TXMAX_FRAMES, 0);
		tw32(HOSTCC_TXCOAL_MAXF_INT, 0);

		for (; i < tp->txq_cnt; i++) {
			u32 reg;

			reg = HOSTCC_TXCOL_TICKS_VEC1 + i * 0x18;
			tw32(reg, ec->tx_coalesce_usecs);
			reg = HOSTCC_TXMAX_FRAMES_VEC1 + i * 0x18;
			tw32(reg, ec->tx_max_coalesced_frames);
			reg = HOSTCC_TXCOAL_MAXF_INT_VEC1 + i * 0x18;
			tw32(reg, ec->tx_max_coalesced_frames_irq);
		}
	}

	for (; i < tp->irq_max - 1; i++) {
		tw32(HOSTCC_TXCOL_TICKS_VEC1 + i * 0x18, 0);
		tw32(HOSTCC_TXMAX_FRAMES_VEC1 + i * 0x18, 0);
		tw32(HOSTCC_TXCOAL_MAXF_INT_VEC1 + i * 0x18, 0);
	}
}
