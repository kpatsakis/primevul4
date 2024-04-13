static int tg3_setup_copper_phy(struct tg3 *tp, int force_reset)
{
	int current_link_up;
	u32 bmsr, val;
	u32 lcl_adv, rmt_adv;
	u16 current_speed;
	u8 current_duplex;
	int i, err;

	tw32(MAC_EVENT, 0);

	tw32_f(MAC_STATUS,
	     (MAC_STATUS_SYNC_CHANGED |
	      MAC_STATUS_CFG_CHANGED |
	      MAC_STATUS_MI_COMPLETION |
	      MAC_STATUS_LNKSTATE_CHANGED));
	udelay(40);

	if ((tp->mi_mode & MAC_MI_MODE_AUTO_POLL) != 0) {
		tw32_f(MAC_MI_MODE,
		     (tp->mi_mode & ~MAC_MI_MODE_AUTO_POLL));
		udelay(80);
	}

	tg3_phy_auxctl_write(tp, MII_TG3_AUXCTL_SHDWSEL_PWRCTL, 0);

	/* Some third-party PHYs need to be reset on link going
	 * down.
	 */
	if ((tg3_asic_rev(tp) == ASIC_REV_5703 ||
	     tg3_asic_rev(tp) == ASIC_REV_5704 ||
	     tg3_asic_rev(tp) == ASIC_REV_5705) &&
	    tp->link_up) {
		tg3_readphy(tp, MII_BMSR, &bmsr);
		if (!tg3_readphy(tp, MII_BMSR, &bmsr) &&
		    !(bmsr & BMSR_LSTATUS))
			force_reset = 1;
	}
	if (force_reset)
		tg3_phy_reset(tp);

	if ((tp->phy_id & TG3_PHY_ID_MASK) == TG3_PHY_ID_BCM5401) {
		tg3_readphy(tp, MII_BMSR, &bmsr);
		if (tg3_readphy(tp, MII_BMSR, &bmsr) ||
		    !tg3_flag(tp, INIT_COMPLETE))
			bmsr = 0;

		if (!(bmsr & BMSR_LSTATUS)) {
			err = tg3_init_5401phy_dsp(tp);
			if (err)
				return err;

			tg3_readphy(tp, MII_BMSR, &bmsr);
			for (i = 0; i < 1000; i++) {
				udelay(10);
				if (!tg3_readphy(tp, MII_BMSR, &bmsr) &&
				    (bmsr & BMSR_LSTATUS)) {
					udelay(40);
					break;
				}
			}

			if ((tp->phy_id & TG3_PHY_ID_REV_MASK) ==
			    TG3_PHY_REV_BCM5401_B0 &&
			    !(bmsr & BMSR_LSTATUS) &&
			    tp->link_config.active_speed == SPEED_1000) {
				err = tg3_phy_reset(tp);
				if (!err)
					err = tg3_init_5401phy_dsp(tp);
				if (err)
					return err;
			}
		}
	} else if (tg3_chip_rev_id(tp) == CHIPREV_ID_5701_A0 ||
		   tg3_chip_rev_id(tp) == CHIPREV_ID_5701_B0) {
		/* 5701 {A0,B0} CRC bug workaround */
		tg3_writephy(tp, 0x15, 0x0a75);
		tg3_writephy(tp, MII_TG3_MISC_SHDW, 0x8c68);
		tg3_writephy(tp, MII_TG3_MISC_SHDW, 0x8d68);
		tg3_writephy(tp, MII_TG3_MISC_SHDW, 0x8c68);
	}

	/* Clear pending interrupts... */
	tg3_readphy(tp, MII_TG3_ISTAT, &val);
	tg3_readphy(tp, MII_TG3_ISTAT, &val);

	if (tp->phy_flags & TG3_PHYFLG_USE_MI_INTERRUPT)
		tg3_writephy(tp, MII_TG3_IMASK, ~MII_TG3_INT_LINKCHG);
	else if (!(tp->phy_flags & TG3_PHYFLG_IS_FET))
		tg3_writephy(tp, MII_TG3_IMASK, ~0);

	if (tg3_asic_rev(tp) == ASIC_REV_5700 ||
	    tg3_asic_rev(tp) == ASIC_REV_5701) {
		if (tp->led_ctrl == LED_CTRL_MODE_PHY_1)
			tg3_writephy(tp, MII_TG3_EXT_CTRL,
				     MII_TG3_EXT_CTRL_LNK3_LED_MODE);
		else
			tg3_writephy(tp, MII_TG3_EXT_CTRL, 0);
	}

	current_link_up = 0;
	current_speed = SPEED_UNKNOWN;
	current_duplex = DUPLEX_UNKNOWN;
	tp->phy_flags &= ~TG3_PHYFLG_MDIX_STATE;
	tp->link_config.rmt_adv = 0;

	if (tp->phy_flags & TG3_PHYFLG_CAPACITIVE_COUPLING) {
		err = tg3_phy_auxctl_read(tp,
					  MII_TG3_AUXCTL_SHDWSEL_MISCTEST,
					  &val);
		if (!err && !(val & (1 << 10))) {
			tg3_phy_auxctl_write(tp,
					     MII_TG3_AUXCTL_SHDWSEL_MISCTEST,
					     val | (1 << 10));
			goto relink;
		}
	}

	bmsr = 0;
	for (i = 0; i < 100; i++) {
		tg3_readphy(tp, MII_BMSR, &bmsr);
		if (!tg3_readphy(tp, MII_BMSR, &bmsr) &&
		    (bmsr & BMSR_LSTATUS))
			break;
		udelay(40);
	}

	if (bmsr & BMSR_LSTATUS) {
		u32 aux_stat, bmcr;

		tg3_readphy(tp, MII_TG3_AUX_STAT, &aux_stat);
		for (i = 0; i < 2000; i++) {
			udelay(10);
			if (!tg3_readphy(tp, MII_TG3_AUX_STAT, &aux_stat) &&
			    aux_stat)
				break;
		}

		tg3_aux_stat_to_speed_duplex(tp, aux_stat,
					     &current_speed,
					     &current_duplex);

		bmcr = 0;
		for (i = 0; i < 200; i++) {
			tg3_readphy(tp, MII_BMCR, &bmcr);
			if (tg3_readphy(tp, MII_BMCR, &bmcr))
				continue;
			if (bmcr && bmcr != 0x7fff)
				break;
			udelay(10);
		}

		lcl_adv = 0;
		rmt_adv = 0;

		tp->link_config.active_speed = current_speed;
		tp->link_config.active_duplex = current_duplex;

		if (tp->link_config.autoneg == AUTONEG_ENABLE) {
			if ((bmcr & BMCR_ANENABLE) &&
			    tg3_phy_copper_an_config_ok(tp, &lcl_adv) &&
			    tg3_phy_copper_fetch_rmtadv(tp, &rmt_adv))
				current_link_up = 1;
		} else {
			if (!(bmcr & BMCR_ANENABLE) &&
			    tp->link_config.speed == current_speed &&
			    tp->link_config.duplex == current_duplex &&
			    tp->link_config.flowctrl ==
			    tp->link_config.active_flowctrl) {
				current_link_up = 1;
			}
		}

		if (current_link_up == 1 &&
		    tp->link_config.active_duplex == DUPLEX_FULL) {
			u32 reg, bit;

			if (tp->phy_flags & TG3_PHYFLG_IS_FET) {
				reg = MII_TG3_FET_GEN_STAT;
				bit = MII_TG3_FET_GEN_STAT_MDIXSTAT;
			} else {
				reg = MII_TG3_EXT_STAT;
				bit = MII_TG3_EXT_STAT_MDIX;
			}

			if (!tg3_readphy(tp, reg, &val) && (val & bit))
				tp->phy_flags |= TG3_PHYFLG_MDIX_STATE;

			tg3_setup_flow_control(tp, lcl_adv, rmt_adv);
		}
	}

relink:
	if (current_link_up == 0 || (tp->phy_flags & TG3_PHYFLG_IS_LOW_POWER)) {
		tg3_phy_copper_begin(tp);

		if (tg3_flag(tp, ROBOSWITCH)) {
			current_link_up = 1;
			/* FIXME: when BCM5325 switch is used use 100 MBit/s */
			current_speed = SPEED_1000;
			current_duplex = DUPLEX_FULL;
			tp->link_config.active_speed = current_speed;
			tp->link_config.active_duplex = current_duplex;
		}

		tg3_readphy(tp, MII_BMSR, &bmsr);
		if ((!tg3_readphy(tp, MII_BMSR, &bmsr) && (bmsr & BMSR_LSTATUS)) ||
		    (tp->mac_mode & MAC_MODE_PORT_INT_LPBACK))
			current_link_up = 1;
	}

	tp->mac_mode &= ~MAC_MODE_PORT_MODE_MASK;
	if (current_link_up == 1) {
		if (tp->link_config.active_speed == SPEED_100 ||
		    tp->link_config.active_speed == SPEED_10)
			tp->mac_mode |= MAC_MODE_PORT_MODE_MII;
		else
			tp->mac_mode |= MAC_MODE_PORT_MODE_GMII;
	} else if (tp->phy_flags & TG3_PHYFLG_IS_FET)
		tp->mac_mode |= MAC_MODE_PORT_MODE_MII;
	else
		tp->mac_mode |= MAC_MODE_PORT_MODE_GMII;

	/* In order for the 5750 core in BCM4785 chip to work properly
	 * in RGMII mode, the Led Control Register must be set up.
	 */
	if (tg3_flag(tp, RGMII_MODE)) {
		u32 led_ctrl = tr32(MAC_LED_CTRL);
		led_ctrl &= ~(LED_CTRL_1000MBPS_ON | LED_CTRL_100MBPS_ON);

		if (tp->link_config.active_speed == SPEED_10)
			led_ctrl |= LED_CTRL_LNKLED_OVERRIDE;
		else if (tp->link_config.active_speed == SPEED_100)
			led_ctrl |= (LED_CTRL_LNKLED_OVERRIDE |
				     LED_CTRL_100MBPS_ON);
		else if (tp->link_config.active_speed == SPEED_1000)
			led_ctrl |= (LED_CTRL_LNKLED_OVERRIDE |
				     LED_CTRL_1000MBPS_ON);

		tw32(MAC_LED_CTRL, led_ctrl);
		udelay(40);
	}

	tp->mac_mode &= ~MAC_MODE_HALF_DUPLEX;
	if (tp->link_config.active_duplex == DUPLEX_HALF)
		tp->mac_mode |= MAC_MODE_HALF_DUPLEX;

	if (tg3_asic_rev(tp) == ASIC_REV_5700) {
		if (current_link_up == 1 &&
		    tg3_5700_link_polarity(tp, tp->link_config.active_speed))
			tp->mac_mode |= MAC_MODE_LINK_POLARITY;
		else
			tp->mac_mode &= ~MAC_MODE_LINK_POLARITY;
	}

	/* ??? Without this setting Netgear GA302T PHY does not
	 * ??? send/receive packets...
	 */
	if ((tp->phy_id & TG3_PHY_ID_MASK) == TG3_PHY_ID_BCM5411 &&
	    tg3_chip_rev_id(tp) == CHIPREV_ID_5700_ALTIMA) {
		tp->mi_mode |= MAC_MI_MODE_AUTO_POLL;
		tw32_f(MAC_MI_MODE, tp->mi_mode);
		udelay(80);
	}

	tw32_f(MAC_MODE, tp->mac_mode);
	udelay(40);

	tg3_phy_eee_adjust(tp, current_link_up);

	if (tg3_flag(tp, USE_LINKCHG_REG)) {
		/* Polled via timer. */
		tw32_f(MAC_EVENT, 0);
	} else {
		tw32_f(MAC_EVENT, MAC_EVENT_LNKSTATE_CHANGED);
	}
	udelay(40);

	if (tg3_asic_rev(tp) == ASIC_REV_5700 &&
	    current_link_up == 1 &&
	    tp->link_config.active_speed == SPEED_1000 &&
	    (tg3_flag(tp, PCIX_MODE) || tg3_flag(tp, PCI_HIGH_SPEED))) {
		udelay(120);
		tw32_f(MAC_STATUS,
		     (MAC_STATUS_SYNC_CHANGED |
		      MAC_STATUS_CFG_CHANGED));
		udelay(40);
		tg3_write_mem(tp,
			      NIC_SRAM_FIRMWARE_MBOX,
			      NIC_SRAM_FIRMWARE_MBOX_MAGIC2);
	}

	/* Prevent send BD corruption. */
	if (tg3_flag(tp, CLKREQ_BUG)) {
		if (tp->link_config.active_speed == SPEED_100 ||
		    tp->link_config.active_speed == SPEED_10)
			pcie_capability_clear_word(tp->pdev, PCI_EXP_LNKCTL,
						   PCI_EXP_LNKCTL_CLKREQ_EN);
		else
			pcie_capability_set_word(tp->pdev, PCI_EXP_LNKCTL,
						 PCI_EXP_LNKCTL_CLKREQ_EN);
	}

	tg3_test_and_report_link_chg(tp, current_link_up);

	return 0;
}
