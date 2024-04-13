static int tg3_setup_phy(struct tg3 *tp, int force_reset)
{
	u32 val;
	int err;

	if (tp->phy_flags & TG3_PHYFLG_PHY_SERDES)
		err = tg3_setup_fiber_phy(tp, force_reset);
	else if (tp->phy_flags & TG3_PHYFLG_MII_SERDES)
		err = tg3_setup_fiber_mii_phy(tp, force_reset);
	else
		err = tg3_setup_copper_phy(tp, force_reset);

	if (tg3_chip_rev(tp) == CHIPREV_5784_AX) {
		u32 scale;

		val = tr32(TG3_CPMU_CLCK_STAT) & CPMU_CLCK_STAT_MAC_CLCK_MASK;
		if (val == CPMU_CLCK_STAT_MAC_CLCK_62_5)
			scale = 65;
		else if (val == CPMU_CLCK_STAT_MAC_CLCK_6_25)
			scale = 6;
		else
			scale = 12;

		val = tr32(GRC_MISC_CFG) & ~GRC_MISC_CFG_PRESCALAR_MASK;
		val |= (scale << GRC_MISC_CFG_PRESCALAR_SHIFT);
		tw32(GRC_MISC_CFG, val);
	}

	val = (2 << TX_LENGTHS_IPG_CRS_SHIFT) |
	      (6 << TX_LENGTHS_IPG_SHIFT);
	if (tg3_asic_rev(tp) == ASIC_REV_5720 ||
	    tg3_asic_rev(tp) == ASIC_REV_5762)
		val |= tr32(MAC_TX_LENGTHS) &
		       (TX_LENGTHS_JMB_FRM_LEN_MSK |
			TX_LENGTHS_CNT_DWN_VAL_MSK);

	if (tp->link_config.active_speed == SPEED_1000 &&
	    tp->link_config.active_duplex == DUPLEX_HALF)
		tw32(MAC_TX_LENGTHS, val |
		     (0xff << TX_LENGTHS_SLOT_TIME_SHIFT));
	else
		tw32(MAC_TX_LENGTHS, val |
		     (32 << TX_LENGTHS_SLOT_TIME_SHIFT));

	if (!tg3_flag(tp, 5705_PLUS)) {
		if (tp->link_up) {
			tw32(HOSTCC_STAT_COAL_TICKS,
			     tp->coal.stats_block_coalesce_usecs);
		} else {
			tw32(HOSTCC_STAT_COAL_TICKS, 0);
		}
	}

	if (tg3_flag(tp, ASPM_WORKAROUND)) {
		val = tr32(PCIE_PWR_MGMT_THRESH);
		if (!tp->link_up)
			val = (val & ~PCIE_PWR_MGMT_L1_THRESH_MSK) |
			      tp->pwrmgmt_thresh;
		else
			val |= PCIE_PWR_MGMT_L1_THRESH_MSK;
		tw32(PCIE_PWR_MGMT_THRESH, val);
	}

	return err;
}
