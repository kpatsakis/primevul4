static bool tg3_phy_copper_fetch_rmtadv(struct tg3 *tp, u32 *rmtadv)
{
	u32 lpeth = 0;

	if (!(tp->phy_flags & TG3_PHYFLG_10_100_ONLY)) {
		u32 val;

		if (tg3_readphy(tp, MII_STAT1000, &val))
			return false;

		lpeth = mii_stat1000_to_ethtool_lpa_t(val);
	}

	if (tg3_readphy(tp, MII_LPA, rmtadv))
		return false;

	lpeth |= mii_lpa_to_ethtool_lpa_t(*rmtadv);
	tp->link_config.rmt_adv = lpeth;

	return true;
}
