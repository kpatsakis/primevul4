static int tg3_setup_fiber_mii_phy(struct tg3 *tp, int force_reset)
{
	int current_link_up, err = 0;
	u32 bmsr, bmcr;
	u16 current_speed;
	u8 current_duplex;
	u32 local_adv, remote_adv;

	tp->mac_mode |= MAC_MODE_PORT_MODE_GMII;
	tw32_f(MAC_MODE, tp->mac_mode);
	udelay(40);

	tw32(MAC_EVENT, 0);

	tw32_f(MAC_STATUS,
	     (MAC_STATUS_SYNC_CHANGED |
	      MAC_STATUS_CFG_CHANGED |
	      MAC_STATUS_MI_COMPLETION |
	      MAC_STATUS_LNKSTATE_CHANGED));
	udelay(40);

	if (force_reset)
		tg3_phy_reset(tp);

	current_link_up = 0;
	current_speed = SPEED_UNKNOWN;
	current_duplex = DUPLEX_UNKNOWN;
	tp->link_config.rmt_adv = 0;

	err |= tg3_readphy(tp, MII_BMSR, &bmsr);
	err |= tg3_readphy(tp, MII_BMSR, &bmsr);
	if (tg3_asic_rev(tp) == ASIC_REV_5714) {
		if (tr32(MAC_TX_STATUS) & TX_STATUS_LINK_UP)
			bmsr |= BMSR_LSTATUS;
		else
			bmsr &= ~BMSR_LSTATUS;
	}

	err |= tg3_readphy(tp, MII_BMCR, &bmcr);

	if ((tp->link_config.autoneg == AUTONEG_ENABLE) && !force_reset &&
	    (tp->phy_flags & TG3_PHYFLG_PARALLEL_DETECT)) {
		/* do nothing, just check for link up at the end */
	} else if (tp->link_config.autoneg == AUTONEG_ENABLE) {
		u32 adv, newadv;

		err |= tg3_readphy(tp, MII_ADVERTISE, &adv);
		newadv = adv & ~(ADVERTISE_1000XFULL | ADVERTISE_1000XHALF |
				 ADVERTISE_1000XPAUSE |
				 ADVERTISE_1000XPSE_ASYM |
				 ADVERTISE_SLCT);

		newadv |= tg3_advert_flowctrl_1000X(tp->link_config.flowctrl);
		newadv |= ethtool_adv_to_mii_adv_x(tp->link_config.advertising);

		if ((newadv != adv) || !(bmcr & BMCR_ANENABLE)) {
			tg3_writephy(tp, MII_ADVERTISE, newadv);
			bmcr |= BMCR_ANENABLE | BMCR_ANRESTART;
			tg3_writephy(tp, MII_BMCR, bmcr);

			tw32_f(MAC_EVENT, MAC_EVENT_LNKSTATE_CHANGED);
			tp->serdes_counter = SERDES_AN_TIMEOUT_5714S;
			tp->phy_flags &= ~TG3_PHYFLG_PARALLEL_DETECT;

			return err;
		}
	} else {
		u32 new_bmcr;

		bmcr &= ~BMCR_SPEED1000;
		new_bmcr = bmcr & ~(BMCR_ANENABLE | BMCR_FULLDPLX);

		if (tp->link_config.duplex == DUPLEX_FULL)
			new_bmcr |= BMCR_FULLDPLX;

		if (new_bmcr != bmcr) {
			/* BMCR_SPEED1000 is a reserved bit that needs
			 * to be set on write.
			 */
			new_bmcr |= BMCR_SPEED1000;

			/* Force a linkdown */
			if (tp->link_up) {
				u32 adv;

				err |= tg3_readphy(tp, MII_ADVERTISE, &adv);
				adv &= ~(ADVERTISE_1000XFULL |
					 ADVERTISE_1000XHALF |
					 ADVERTISE_SLCT);
				tg3_writephy(tp, MII_ADVERTISE, adv);
				tg3_writephy(tp, MII_BMCR, bmcr |
							   BMCR_ANRESTART |
							   BMCR_ANENABLE);
				udelay(10);
				tg3_carrier_off(tp);
			}
			tg3_writephy(tp, MII_BMCR, new_bmcr);
			bmcr = new_bmcr;
			err |= tg3_readphy(tp, MII_BMSR, &bmsr);
			err |= tg3_readphy(tp, MII_BMSR, &bmsr);
			if (tg3_asic_rev(tp) == ASIC_REV_5714) {
				if (tr32(MAC_TX_STATUS) & TX_STATUS_LINK_UP)
					bmsr |= BMSR_LSTATUS;
				else
					bmsr &= ~BMSR_LSTATUS;
			}
			tp->phy_flags &= ~TG3_PHYFLG_PARALLEL_DETECT;
		}
	}

	if (bmsr & BMSR_LSTATUS) {
		current_speed = SPEED_1000;
		current_link_up = 1;
		if (bmcr & BMCR_FULLDPLX)
			current_duplex = DUPLEX_FULL;
		else
			current_duplex = DUPLEX_HALF;

		local_adv = 0;
		remote_adv = 0;

		if (bmcr & BMCR_ANENABLE) {
			u32 common;

			err |= tg3_readphy(tp, MII_ADVERTISE, &local_adv);
			err |= tg3_readphy(tp, MII_LPA, &remote_adv);
			common = local_adv & remote_adv;
			if (common & (ADVERTISE_1000XHALF |
				      ADVERTISE_1000XFULL)) {
				if (common & ADVERTISE_1000XFULL)
					current_duplex = DUPLEX_FULL;
				else
					current_duplex = DUPLEX_HALF;

				tp->link_config.rmt_adv =
					   mii_adv_to_ethtool_adv_x(remote_adv);
			} else if (!tg3_flag(tp, 5780_CLASS)) {
				/* Link is up via parallel detect */
			} else {
				current_link_up = 0;
			}
		}
	}

	if (current_link_up == 1 && current_duplex == DUPLEX_FULL)
		tg3_setup_flow_control(tp, local_adv, remote_adv);

	tp->mac_mode &= ~MAC_MODE_HALF_DUPLEX;
	if (tp->link_config.active_duplex == DUPLEX_HALF)
		tp->mac_mode |= MAC_MODE_HALF_DUPLEX;

	tw32_f(MAC_MODE, tp->mac_mode);
	udelay(40);

	tw32_f(MAC_EVENT, MAC_EVENT_LNKSTATE_CHANGED);

	tp->link_config.active_speed = current_speed;
	tp->link_config.active_duplex = current_duplex;

	tg3_test_and_report_link_chg(tp, current_link_up);
	return err;
}
