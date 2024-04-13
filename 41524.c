static void tg3_setup_rxbd_thresholds(struct tg3 *tp)
{
	u32 val, bdcache_maxcnt, host_rep_thresh, nic_rep_thresh;

	if (!tg3_flag(tp, 5750_PLUS) ||
	    tg3_flag(tp, 5780_CLASS) ||
	    tg3_asic_rev(tp) == ASIC_REV_5750 ||
	    tg3_asic_rev(tp) == ASIC_REV_5752 ||
	    tg3_flag(tp, 57765_PLUS))
		bdcache_maxcnt = TG3_SRAM_RX_STD_BDCACHE_SIZE_5700;
	else if (tg3_asic_rev(tp) == ASIC_REV_5755 ||
		 tg3_asic_rev(tp) == ASIC_REV_5787)
		bdcache_maxcnt = TG3_SRAM_RX_STD_BDCACHE_SIZE_5755;
	else
		bdcache_maxcnt = TG3_SRAM_RX_STD_BDCACHE_SIZE_5906;

	nic_rep_thresh = min(bdcache_maxcnt / 2, tp->rx_std_max_post);
	host_rep_thresh = max_t(u32, tp->rx_pending / 8, 1);

	val = min(nic_rep_thresh, host_rep_thresh);
	tw32(RCVBDI_STD_THRESH, val);

	if (tg3_flag(tp, 57765_PLUS))
		tw32(STD_REPLENISH_LWM, bdcache_maxcnt);

	if (!tg3_flag(tp, JUMBO_CAPABLE) || tg3_flag(tp, 5780_CLASS))
		return;

	bdcache_maxcnt = TG3_SRAM_RX_JMB_BDCACHE_SIZE_5700;

	host_rep_thresh = max_t(u32, tp->rx_jumbo_pending / 8, 1);

	val = min(bdcache_maxcnt / 2, host_rep_thresh);
	tw32(RCVBDI_JUMBO_THRESH, val);

	if (tg3_flag(tp, 57765_PLUS))
		tw32(JMB_REPLENISH_LWM, bdcache_maxcnt);
}
