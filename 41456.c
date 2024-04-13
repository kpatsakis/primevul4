static void tg3_power_down_phy(struct tg3 *tp, bool do_low_power)
{
	u32 val;

	if (tp->phy_flags & TG3_PHYFLG_PHY_SERDES) {
		if (tg3_asic_rev(tp) == ASIC_REV_5704) {
			u32 sg_dig_ctrl = tr32(SG_DIG_CTRL);
			u32 serdes_cfg = tr32(MAC_SERDES_CFG);

			sg_dig_ctrl |=
				SG_DIG_USING_HW_AUTONEG | SG_DIG_SOFT_RESET;
			tw32(SG_DIG_CTRL, sg_dig_ctrl);
			tw32(MAC_SERDES_CFG, serdes_cfg | (1 << 15));
		}
		return;
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5906) {
		tg3_bmcr_reset(tp);
		val = tr32(GRC_MISC_CFG);
		tw32_f(GRC_MISC_CFG, val | GRC_MISC_CFG_EPHY_IDDQ);
		udelay(40);
		return;
	} else if (tp->phy_flags & TG3_PHYFLG_IS_FET) {
		u32 phytest;
		if (!tg3_readphy(tp, MII_TG3_FET_TEST, &phytest)) {
			u32 phy;

			tg3_writephy(tp, MII_ADVERTISE, 0);
			tg3_writephy(tp, MII_BMCR,
				     BMCR_ANENABLE | BMCR_ANRESTART);

			tg3_writephy(tp, MII_TG3_FET_TEST,
				     phytest | MII_TG3_FET_SHADOW_EN);
			if (!tg3_readphy(tp, MII_TG3_FET_SHDW_AUXMODE4, &phy)) {
				phy |= MII_TG3_FET_SHDW_AUXMODE4_SBPD;
				tg3_writephy(tp,
					     MII_TG3_FET_SHDW_AUXMODE4,
					     phy);
			}
			tg3_writephy(tp, MII_TG3_FET_TEST, phytest);
		}
		return;
	} else if (do_low_power) {
		tg3_writephy(tp, MII_TG3_EXT_CTRL,
			     MII_TG3_EXT_CTRL_FORCE_LED_OFF);

		val = MII_TG3_AUXCTL_PCTL_100TX_LPWR |
		      MII_TG3_AUXCTL_PCTL_SPR_ISOLATE |
		      MII_TG3_AUXCTL_PCTL_VREG_11V;
		tg3_phy_auxctl_write(tp, MII_TG3_AUXCTL_SHDWSEL_PWRCTL, val);
	}

	/* The PHY should not be powered down on some chips because
	 * of bugs.
	 */
	if (tg3_asic_rev(tp) == ASIC_REV_5700 ||
	    tg3_asic_rev(tp) == ASIC_REV_5704 ||
	    (tg3_asic_rev(tp) == ASIC_REV_5780 &&
	     (tp->phy_flags & TG3_PHYFLG_MII_SERDES)) ||
	    (tg3_asic_rev(tp) == ASIC_REV_5717 &&
	     !tp->pci_fn))
		return;

	if (tg3_chip_rev(tp) == CHIPREV_5784_AX ||
	    tg3_chip_rev(tp) == CHIPREV_5761_AX) {
		val = tr32(TG3_CPMU_LSPD_1000MB_CLK);
		val &= ~CPMU_LSPD_1000MB_MACCLK_MASK;
		val |= CPMU_LSPD_1000MB_MACCLK_12_5;
		tw32_f(TG3_CPMU_LSPD_1000MB_CLK, val);
	}

	tg3_writephy(tp, MII_BMCR, BMCR_PDOWN);
}
