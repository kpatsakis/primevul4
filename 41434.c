static void tg3_phy_eee_adjust(struct tg3 *tp, u32 current_link_up)
{
	u32 val;

	if (!(tp->phy_flags & TG3_PHYFLG_EEE_CAP))
		return;

	tp->setlpicnt = 0;

	if (tp->link_config.autoneg == AUTONEG_ENABLE &&
	    current_link_up == 1 &&
	    tp->link_config.active_duplex == DUPLEX_FULL &&
	    (tp->link_config.active_speed == SPEED_100 ||
	     tp->link_config.active_speed == SPEED_1000)) {
		u32 eeectl;

		if (tp->link_config.active_speed == SPEED_1000)
			eeectl = TG3_CPMU_EEE_CTRL_EXIT_16_5_US;
		else
			eeectl = TG3_CPMU_EEE_CTRL_EXIT_36_US;

		tw32(TG3_CPMU_EEE_CTRL, eeectl);

		tg3_phy_cl45_read(tp, MDIO_MMD_AN,
				  TG3_CL45_D7_EEERES_STAT, &val);

		if (val == TG3_CL45_D7_EEERES_STAT_LP_1000T ||
		    val == TG3_CL45_D7_EEERES_STAT_LP_100TX)
			tp->setlpicnt = 2;
	}

	if (!tp->setlpicnt) {
		if (current_link_up == 1 &&
		   !tg3_phy_toggle_auxctl_smdsp(tp, true)) {
			tg3_phydsp_write(tp, MII_TG3_DSP_TAP26, 0x0000);
			tg3_phy_toggle_auxctl_smdsp(tp, false);
		}

		val = tr32(TG3_CPMU_EEE_MODE);
		tw32(TG3_CPMU_EEE_MODE, val & ~TG3_CPMU_EEEMD_LPI_ENABLE);
	}
}
