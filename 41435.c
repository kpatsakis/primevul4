static void tg3_phy_eee_enable(struct tg3 *tp)
{
	u32 val;

	if (tp->link_config.active_speed == SPEED_1000 &&
	    (tg3_asic_rev(tp) == ASIC_REV_5717 ||
	     tg3_asic_rev(tp) == ASIC_REV_5719 ||
	     tg3_flag(tp, 57765_CLASS)) &&
	    !tg3_phy_toggle_auxctl_smdsp(tp, true)) {
		val = MII_TG3_DSP_TAP26_ALNOKO |
		      MII_TG3_DSP_TAP26_RMRXSTO;
		tg3_phydsp_write(tp, MII_TG3_DSP_TAP26, val);
		tg3_phy_toggle_auxctl_smdsp(tp, false);
	}

	val = tr32(TG3_CPMU_EEE_MODE);
	tw32(TG3_CPMU_EEE_MODE, val | TG3_CPMU_EEEMD_LPI_ENABLE);
}
