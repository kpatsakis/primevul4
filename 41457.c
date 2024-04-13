static int tg3_power_down_prepare(struct tg3 *tp)
{
	u32 misc_host_ctrl;
	bool device_should_wake, do_low_power;

	tg3_enable_register_access(tp);

	/* Restore the CLKREQ setting. */
	if (tg3_flag(tp, CLKREQ_BUG))
		pcie_capability_set_word(tp->pdev, PCI_EXP_LNKCTL,
					 PCI_EXP_LNKCTL_CLKREQ_EN);

	misc_host_ctrl = tr32(TG3PCI_MISC_HOST_CTRL);
	tw32(TG3PCI_MISC_HOST_CTRL,
	     misc_host_ctrl | MISC_HOST_CTRL_MASK_PCI_INT);

	device_should_wake = device_may_wakeup(&tp->pdev->dev) &&
			     tg3_flag(tp, WOL_ENABLE);

	if (tg3_flag(tp, USE_PHYLIB)) {
		do_low_power = false;
		if ((tp->phy_flags & TG3_PHYFLG_IS_CONNECTED) &&
		    !(tp->phy_flags & TG3_PHYFLG_IS_LOW_POWER)) {
			struct phy_device *phydev;
			u32 phyid, advertising;

			phydev = tp->mdio_bus->phy_map[TG3_PHY_MII_ADDR];

			tp->phy_flags |= TG3_PHYFLG_IS_LOW_POWER;

			tp->link_config.speed = phydev->speed;
			tp->link_config.duplex = phydev->duplex;
			tp->link_config.autoneg = phydev->autoneg;
			tp->link_config.advertising = phydev->advertising;

			advertising = ADVERTISED_TP |
				      ADVERTISED_Pause |
				      ADVERTISED_Autoneg |
				      ADVERTISED_10baseT_Half;

			if (tg3_flag(tp, ENABLE_ASF) || device_should_wake) {
				if (tg3_flag(tp, WOL_SPEED_100MB))
					advertising |=
						ADVERTISED_100baseT_Half |
						ADVERTISED_100baseT_Full |
						ADVERTISED_10baseT_Full;
				else
					advertising |= ADVERTISED_10baseT_Full;
			}

			phydev->advertising = advertising;

			phy_start_aneg(phydev);

			phyid = phydev->drv->phy_id & phydev->drv->phy_id_mask;
			if (phyid != PHY_ID_BCMAC131) {
				phyid &= PHY_BCM_OUI_MASK;
				if (phyid == PHY_BCM_OUI_1 ||
				    phyid == PHY_BCM_OUI_2 ||
				    phyid == PHY_BCM_OUI_3)
					do_low_power = true;
			}
		}
	} else {
		do_low_power = true;

		if (!(tp->phy_flags & TG3_PHYFLG_IS_LOW_POWER))
			tp->phy_flags |= TG3_PHYFLG_IS_LOW_POWER;

		if (!(tp->phy_flags & TG3_PHYFLG_ANY_SERDES))
			tg3_setup_phy(tp, 0);
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5906) {
		u32 val;

		val = tr32(GRC_VCPU_EXT_CTRL);
		tw32(GRC_VCPU_EXT_CTRL, val | GRC_VCPU_EXT_CTRL_DISABLE_WOL);
	} else if (!tg3_flag(tp, ENABLE_ASF)) {
		int i;
		u32 val;

		for (i = 0; i < 200; i++) {
			tg3_read_mem(tp, NIC_SRAM_FW_ASF_STATUS_MBOX, &val);
			if (val == ~NIC_SRAM_FIRMWARE_MBOX_MAGIC1)
				break;
			msleep(1);
		}
	}
	if (tg3_flag(tp, WOL_CAP))
		tg3_write_mem(tp, NIC_SRAM_WOL_MBOX, WOL_SIGNATURE |
						     WOL_DRV_STATE_SHUTDOWN |
						     WOL_DRV_WOL |
						     WOL_SET_MAGIC_PKT);

	if (device_should_wake) {
		u32 mac_mode;

		if (!(tp->phy_flags & TG3_PHYFLG_PHY_SERDES)) {
			if (do_low_power &&
			    !(tp->phy_flags & TG3_PHYFLG_IS_FET)) {
				tg3_phy_auxctl_write(tp,
					       MII_TG3_AUXCTL_SHDWSEL_PWRCTL,
					       MII_TG3_AUXCTL_PCTL_WOL_EN |
					       MII_TG3_AUXCTL_PCTL_100TX_LPWR |
					       MII_TG3_AUXCTL_PCTL_CL_AB_TXDAC);
				udelay(40);
			}

			if (tp->phy_flags & TG3_PHYFLG_MII_SERDES)
				mac_mode = MAC_MODE_PORT_MODE_GMII;
			else
				mac_mode = MAC_MODE_PORT_MODE_MII;

			mac_mode |= tp->mac_mode & MAC_MODE_LINK_POLARITY;
			if (tg3_asic_rev(tp) == ASIC_REV_5700) {
				u32 speed = tg3_flag(tp, WOL_SPEED_100MB) ?
					     SPEED_100 : SPEED_10;
				if (tg3_5700_link_polarity(tp, speed))
					mac_mode |= MAC_MODE_LINK_POLARITY;
				else
					mac_mode &= ~MAC_MODE_LINK_POLARITY;
			}
		} else {
			mac_mode = MAC_MODE_PORT_MODE_TBI;
		}

		if (!tg3_flag(tp, 5750_PLUS))
			tw32(MAC_LED_CTRL, tp->led_ctrl);

		mac_mode |= MAC_MODE_MAGIC_PKT_ENABLE;
		if ((tg3_flag(tp, 5705_PLUS) && !tg3_flag(tp, 5780_CLASS)) &&
		    (tg3_flag(tp, ENABLE_ASF) || tg3_flag(tp, ENABLE_APE)))
			mac_mode |= MAC_MODE_KEEP_FRAME_IN_WOL;

		if (tg3_flag(tp, ENABLE_APE))
			mac_mode |= MAC_MODE_APE_TX_EN |
				    MAC_MODE_APE_RX_EN |
				    MAC_MODE_TDE_ENABLE;

		tw32_f(MAC_MODE, mac_mode);
		udelay(100);

		tw32_f(MAC_RX_MODE, RX_MODE_ENABLE);
		udelay(10);
	}

	if (!tg3_flag(tp, WOL_SPEED_100MB) &&
	    (tg3_asic_rev(tp) == ASIC_REV_5700 ||
	     tg3_asic_rev(tp) == ASIC_REV_5701)) {
		u32 base_val;

		base_val = tp->pci_clock_ctrl;
		base_val |= (CLOCK_CTRL_RXCLK_DISABLE |
			     CLOCK_CTRL_TXCLK_DISABLE);

		tw32_wait_f(TG3PCI_CLOCK_CTRL, base_val | CLOCK_CTRL_ALTCLK |
			    CLOCK_CTRL_PWRDOWN_PLL133, 40);
	} else if (tg3_flag(tp, 5780_CLASS) ||
		   tg3_flag(tp, CPMU_PRESENT) ||
		   tg3_asic_rev(tp) == ASIC_REV_5906) {
		/* do nothing */
	} else if (!(tg3_flag(tp, 5750_PLUS) && tg3_flag(tp, ENABLE_ASF))) {
		u32 newbits1, newbits2;

		if (tg3_asic_rev(tp) == ASIC_REV_5700 ||
		    tg3_asic_rev(tp) == ASIC_REV_5701) {
			newbits1 = (CLOCK_CTRL_RXCLK_DISABLE |
				    CLOCK_CTRL_TXCLK_DISABLE |
				    CLOCK_CTRL_ALTCLK);
			newbits2 = newbits1 | CLOCK_CTRL_44MHZ_CORE;
		} else if (tg3_flag(tp, 5705_PLUS)) {
			newbits1 = CLOCK_CTRL_625_CORE;
			newbits2 = newbits1 | CLOCK_CTRL_ALTCLK;
		} else {
			newbits1 = CLOCK_CTRL_ALTCLK;
			newbits2 = newbits1 | CLOCK_CTRL_44MHZ_CORE;
		}

		tw32_wait_f(TG3PCI_CLOCK_CTRL, tp->pci_clock_ctrl | newbits1,
			    40);

		tw32_wait_f(TG3PCI_CLOCK_CTRL, tp->pci_clock_ctrl | newbits2,
			    40);

		if (!tg3_flag(tp, 5705_PLUS)) {
			u32 newbits3;

			if (tg3_asic_rev(tp) == ASIC_REV_5700 ||
			    tg3_asic_rev(tp) == ASIC_REV_5701) {
				newbits3 = (CLOCK_CTRL_RXCLK_DISABLE |
					    CLOCK_CTRL_TXCLK_DISABLE |
					    CLOCK_CTRL_44MHZ_CORE);
			} else {
				newbits3 = CLOCK_CTRL_44MHZ_CORE;
			}

			tw32_wait_f(TG3PCI_CLOCK_CTRL,
				    tp->pci_clock_ctrl | newbits3, 40);
		}
	}

	if (!(device_should_wake) && !tg3_flag(tp, ENABLE_ASF))
		tg3_power_down_phy(tp, do_low_power);

	tg3_frob_aux_power(tp, true);

	/* Workaround for unstable PLL clock */
	if ((!tg3_flag(tp, IS_SSB_CORE)) &&
	    ((tg3_chip_rev(tp) == CHIPREV_5750_AX) ||
	     (tg3_chip_rev(tp) == CHIPREV_5750_BX))) {
		u32 val = tr32(0x7d00);

		val &= ~((1 << 16) | (1 << 4) | (1 << 2) | (1 << 1) | 1);
		tw32(0x7d00, val);
		if (!tg3_flag(tp, ENABLE_ASF)) {
			int err;

			err = tg3_nvram_lock(tp);
			tg3_halt_cpu(tp, RX_CPU_BASE);
			if (!err)
				tg3_nvram_unlock(tp);
		}
	}

	tg3_write_sig_post_reset(tp, RESET_KIND_SHUTDOWN);

	return 0;
}
