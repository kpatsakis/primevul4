static void tg3_mac_loopback(struct tg3 *tp, bool enable)
{
	if (enable) {
		tp->mac_mode &= ~(MAC_MODE_HALF_DUPLEX |
				  MAC_MODE_PORT_MODE_MASK);

		tp->mac_mode |= MAC_MODE_PORT_INT_LPBACK;

		if (!tg3_flag(tp, 5705_PLUS))
			tp->mac_mode |= MAC_MODE_LINK_POLARITY;

		if (tp->phy_flags & TG3_PHYFLG_10_100_ONLY)
			tp->mac_mode |= MAC_MODE_PORT_MODE_MII;
		else
			tp->mac_mode |= MAC_MODE_PORT_MODE_GMII;
	} else {
		tp->mac_mode &= ~MAC_MODE_PORT_INT_LPBACK;

		if (tg3_flag(tp, 5705_PLUS) ||
		    (tp->phy_flags & TG3_PHYFLG_PHY_SERDES) ||
		    tg3_asic_rev(tp) == ASIC_REV_5700)
			tp->mac_mode &= ~MAC_MODE_LINK_POLARITY;
	}

	tw32(MAC_MODE, tp->mac_mode);
	udelay(40);
}
