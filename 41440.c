static int tg3_phy_reset(struct tg3 *tp)
{
	u32 val, cpmuctrl;
	int err;

	if (tg3_asic_rev(tp) == ASIC_REV_5906) {
		val = tr32(GRC_MISC_CFG);
		tw32_f(GRC_MISC_CFG, val & ~GRC_MISC_CFG_EPHY_IDDQ);
		udelay(40);
	}
	err  = tg3_readphy(tp, MII_BMSR, &val);
	err |= tg3_readphy(tp, MII_BMSR, &val);
	if (err != 0)
		return -EBUSY;

	if (netif_running(tp->dev) && tp->link_up) {
		netif_carrier_off(tp->dev);
		tg3_link_report(tp);
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5703 ||
	    tg3_asic_rev(tp) == ASIC_REV_5704 ||
	    tg3_asic_rev(tp) == ASIC_REV_5705) {
		err = tg3_phy_reset_5703_4_5(tp);
		if (err)
			return err;
		goto out;
	}

	cpmuctrl = 0;
	if (tg3_asic_rev(tp) == ASIC_REV_5784 &&
	    tg3_chip_rev(tp) != CHIPREV_5784_AX) {
		cpmuctrl = tr32(TG3_CPMU_CTRL);
		if (cpmuctrl & CPMU_CTRL_GPHY_10MB_RXONLY)
			tw32(TG3_CPMU_CTRL,
			     cpmuctrl & ~CPMU_CTRL_GPHY_10MB_RXONLY);
	}

	err = tg3_bmcr_reset(tp);
	if (err)
		return err;

	if (cpmuctrl & CPMU_CTRL_GPHY_10MB_RXONLY) {
		val = MII_TG3_DSP_EXP8_AEDW | MII_TG3_DSP_EXP8_REJ2MHz;
		tg3_phydsp_write(tp, MII_TG3_DSP_EXP8, val);

		tw32(TG3_CPMU_CTRL, cpmuctrl);
	}

	if (tg3_chip_rev(tp) == CHIPREV_5784_AX ||
	    tg3_chip_rev(tp) == CHIPREV_5761_AX) {
		val = tr32(TG3_CPMU_LSPD_1000MB_CLK);
		if ((val & CPMU_LSPD_1000MB_MACCLK_MASK) ==
		    CPMU_LSPD_1000MB_MACCLK_12_5) {
			val &= ~CPMU_LSPD_1000MB_MACCLK_MASK;
			udelay(40);
			tw32_f(TG3_CPMU_LSPD_1000MB_CLK, val);
		}
	}

	if (tg3_flag(tp, 5717_PLUS) &&
	    (tp->phy_flags & TG3_PHYFLG_MII_SERDES))
		return 0;

	tg3_phy_apply_otp(tp);

	if (tp->phy_flags & TG3_PHYFLG_ENABLE_APD)
		tg3_phy_toggle_apd(tp, true);
	else
		tg3_phy_toggle_apd(tp, false);

out:
	if ((tp->phy_flags & TG3_PHYFLG_ADC_BUG) &&
	    !tg3_phy_toggle_auxctl_smdsp(tp, true)) {
		tg3_phydsp_write(tp, 0x201f, 0x2aaa);
		tg3_phydsp_write(tp, 0x000a, 0x0323);
		tg3_phy_toggle_auxctl_smdsp(tp, false);
	}

	if (tp->phy_flags & TG3_PHYFLG_5704_A0_BUG) {
		tg3_writephy(tp, MII_TG3_MISC_SHDW, 0x8d68);
		tg3_writephy(tp, MII_TG3_MISC_SHDW, 0x8d68);
	}

	if (tp->phy_flags & TG3_PHYFLG_BER_BUG) {
		if (!tg3_phy_toggle_auxctl_smdsp(tp, true)) {
			tg3_phydsp_write(tp, 0x000a, 0x310b);
			tg3_phydsp_write(tp, 0x201f, 0x9506);
			tg3_phydsp_write(tp, 0x401f, 0x14e2);
			tg3_phy_toggle_auxctl_smdsp(tp, false);
		}
	} else if (tp->phy_flags & TG3_PHYFLG_JITTER_BUG) {
		if (!tg3_phy_toggle_auxctl_smdsp(tp, true)) {
			tg3_writephy(tp, MII_TG3_DSP_ADDRESS, 0x000a);
			if (tp->phy_flags & TG3_PHYFLG_ADJUST_TRIM) {
				tg3_writephy(tp, MII_TG3_DSP_RW_PORT, 0x110b);
				tg3_writephy(tp, MII_TG3_TEST1,
					     MII_TG3_TEST1_TRIM_EN | 0x4);
			} else
				tg3_writephy(tp, MII_TG3_DSP_RW_PORT, 0x010b);

			tg3_phy_toggle_auxctl_smdsp(tp, false);
		}
	}

	/* Set Extended packet length bit (bit 14) on all chips that */
	/* support jumbo frames */
	if ((tp->phy_id & TG3_PHY_ID_MASK) == TG3_PHY_ID_BCM5401) {
		/* Cannot do read-modify-write on 5401 */
		tg3_phy_auxctl_write(tp, MII_TG3_AUXCTL_SHDWSEL_AUXCTL, 0x4c20);
	} else if (tg3_flag(tp, JUMBO_CAPABLE)) {
		/* Set bit 14 with read-modify-write to preserve other bits */
		err = tg3_phy_auxctl_read(tp,
					  MII_TG3_AUXCTL_SHDWSEL_AUXCTL, &val);
		if (!err)
			tg3_phy_auxctl_write(tp, MII_TG3_AUXCTL_SHDWSEL_AUXCTL,
					   val | MII_TG3_AUXCTL_ACTL_EXTPKTLEN);
	}

	/* Set phy register 0x10 bit 0 to high fifo elasticity to support
	 * jumbo frames transmission.
	 */
	if (tg3_flag(tp, JUMBO_CAPABLE)) {
		if (!tg3_readphy(tp, MII_TG3_EXT_CTRL, &val))
			tg3_writephy(tp, MII_TG3_EXT_CTRL,
				     val | MII_TG3_EXT_CTRL_FIFO_ELASTIC);
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5906) {
		/* adjust output voltage */
		tg3_writephy(tp, MII_TG3_FET_PTEST, 0x12);
	}

	if (tg3_chip_rev_id(tp) == CHIPREV_ID_5762_A0)
		tg3_phydsp_write(tp, 0xffb, 0x4000);

	tg3_phy_toggle_automdix(tp, 1);
	tg3_phy_set_wirespeed(tp);
	return 0;
}
