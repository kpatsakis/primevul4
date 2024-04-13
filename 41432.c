static void tg3_phy_copper_begin(struct tg3 *tp)
{
	if (tp->link_config.autoneg == AUTONEG_ENABLE ||
	    (tp->phy_flags & TG3_PHYFLG_IS_LOW_POWER)) {
		u32 adv, fc;

		if (tp->phy_flags & TG3_PHYFLG_IS_LOW_POWER) {
			adv = ADVERTISED_10baseT_Half |
			      ADVERTISED_10baseT_Full;
			if (tg3_flag(tp, WOL_SPEED_100MB))
				adv |= ADVERTISED_100baseT_Half |
				       ADVERTISED_100baseT_Full;

			fc = FLOW_CTRL_TX | FLOW_CTRL_RX;
		} else {
			adv = tp->link_config.advertising;
			if (tp->phy_flags & TG3_PHYFLG_10_100_ONLY)
				adv &= ~(ADVERTISED_1000baseT_Half |
					 ADVERTISED_1000baseT_Full);

			fc = tp->link_config.flowctrl;
		}

		tg3_phy_autoneg_cfg(tp, adv, fc);

		tg3_writephy(tp, MII_BMCR,
			     BMCR_ANENABLE | BMCR_ANRESTART);
	} else {
		int i;
		u32 bmcr, orig_bmcr;

		tp->link_config.active_speed = tp->link_config.speed;
		tp->link_config.active_duplex = tp->link_config.duplex;

		if (tg3_asic_rev(tp) == ASIC_REV_5714) {
			/* With autoneg disabled, 5715 only links up when the
			 * advertisement register has the configured speed
			 * enabled.
			 */
			tg3_writephy(tp, MII_ADVERTISE, ADVERTISE_ALL);
		}

		bmcr = 0;
		switch (tp->link_config.speed) {
		default:
		case SPEED_10:
			break;

		case SPEED_100:
			bmcr |= BMCR_SPEED100;
			break;

		case SPEED_1000:
			bmcr |= BMCR_SPEED1000;
			break;
		}

		if (tp->link_config.duplex == DUPLEX_FULL)
			bmcr |= BMCR_FULLDPLX;

		if (!tg3_readphy(tp, MII_BMCR, &orig_bmcr) &&
		    (bmcr != orig_bmcr)) {
			tg3_writephy(tp, MII_BMCR, BMCR_LOOPBACK);
			for (i = 0; i < 1500; i++) {
				u32 tmp;

				udelay(10);
				if (tg3_readphy(tp, MII_BMSR, &tmp) ||
				    tg3_readphy(tp, MII_BMSR, &tmp))
					continue;
				if (!(tmp & BMSR_LSTATUS)) {
					udelay(40);
					break;
				}
			}
			tg3_writephy(tp, MII_BMCR, bmcr);
			udelay(40);
		}
	}
}
