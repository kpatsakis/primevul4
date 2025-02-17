static int tg3_phy_lpbk_set(struct tg3 *tp, u32 speed, bool extlpbk)
{
	u32 val, bmcr, mac_mode, ptest = 0;

	tg3_phy_toggle_apd(tp, false);
	tg3_phy_toggle_automdix(tp, 0);

	if (extlpbk && tg3_phy_set_extloopbk(tp))
		return -EIO;

	bmcr = BMCR_FULLDPLX;
	switch (speed) {
	case SPEED_10:
		break;
	case SPEED_100:
		bmcr |= BMCR_SPEED100;
		break;
	case SPEED_1000:
	default:
		if (tp->phy_flags & TG3_PHYFLG_IS_FET) {
			speed = SPEED_100;
			bmcr |= BMCR_SPEED100;
		} else {
			speed = SPEED_1000;
			bmcr |= BMCR_SPEED1000;
		}
	}

	if (extlpbk) {
		if (!(tp->phy_flags & TG3_PHYFLG_IS_FET)) {
			tg3_readphy(tp, MII_CTRL1000, &val);
			val |= CTL1000_AS_MASTER |
			       CTL1000_ENABLE_MASTER;
			tg3_writephy(tp, MII_CTRL1000, val);
		} else {
			ptest = MII_TG3_FET_PTEST_TRIM_SEL |
				MII_TG3_FET_PTEST_TRIM_2;
			tg3_writephy(tp, MII_TG3_FET_PTEST, ptest);
		}
	} else
		bmcr |= BMCR_LOOPBACK;

	tg3_writephy(tp, MII_BMCR, bmcr);

	/* The write needs to be flushed for the FETs */
	if (tp->phy_flags & TG3_PHYFLG_IS_FET)
		tg3_readphy(tp, MII_BMCR, &bmcr);

	udelay(40);

	if ((tp->phy_flags & TG3_PHYFLG_IS_FET) &&
	    tg3_asic_rev(tp) == ASIC_REV_5785) {
		tg3_writephy(tp, MII_TG3_FET_PTEST, ptest |
			     MII_TG3_FET_PTEST_FRC_TX_LINK |
			     MII_TG3_FET_PTEST_FRC_TX_LOCK);

		/* The write needs to be flushed for the AC131 */
		tg3_readphy(tp, MII_TG3_FET_PTEST, &val);
	}

	/* Reset to prevent losing 1st rx packet intermittently */
	if ((tp->phy_flags & TG3_PHYFLG_MII_SERDES) &&
	    tg3_flag(tp, 5780_CLASS)) {
		tw32_f(MAC_RX_MODE, RX_MODE_RESET);
		udelay(10);
		tw32_f(MAC_RX_MODE, tp->rx_mode);
	}

	mac_mode = tp->mac_mode &
		   ~(MAC_MODE_PORT_MODE_MASK | MAC_MODE_HALF_DUPLEX);
	if (speed == SPEED_1000)
		mac_mode |= MAC_MODE_PORT_MODE_GMII;
	else
		mac_mode |= MAC_MODE_PORT_MODE_MII;

	if (tg3_asic_rev(tp) == ASIC_REV_5700) {
		u32 masked_phy_id = tp->phy_id & TG3_PHY_ID_MASK;

		if (masked_phy_id == TG3_PHY_ID_BCM5401)
			mac_mode &= ~MAC_MODE_LINK_POLARITY;
		else if (masked_phy_id == TG3_PHY_ID_BCM5411)
			mac_mode |= MAC_MODE_LINK_POLARITY;

		tg3_writephy(tp, MII_TG3_EXT_CTRL,
			     MII_TG3_EXT_CTRL_LNK3_LED_MODE);
	}

	tw32(MAC_MODE, mac_mode);
	udelay(40);

	return 0;
}
