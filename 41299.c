static u64 tg3_calc_crc_errors(struct tg3 *tp)
{
	struct tg3_hw_stats *hw_stats = tp->hw_stats;

	if (!(tp->phy_flags & TG3_PHYFLG_PHY_SERDES) &&
	    (tg3_asic_rev(tp) == ASIC_REV_5700 ||
	     tg3_asic_rev(tp) == ASIC_REV_5701)) {
		u32 val;

		if (!tg3_readphy(tp, MII_TG3_TEST1, &val)) {
			tg3_writephy(tp, MII_TG3_TEST1,
				     val | MII_TG3_TEST1_CRC_EN);
			tg3_readphy(tp, MII_TG3_RXR_COUNTERS, &val);
		} else
			val = 0;

		tp->phy_crc_errors += val;

		return tp->phy_crc_errors;
	}

	return get_stat64(&hw_stats->rx_fcs_errors);
}
