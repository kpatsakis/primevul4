static void tg3_phy_init_link_config(struct tg3 *tp)
{
	u32 adv = ADVERTISED_Autoneg;

	if (!(tp->phy_flags & TG3_PHYFLG_10_100_ONLY))
		adv |= ADVERTISED_1000baseT_Half |
		       ADVERTISED_1000baseT_Full;

	if (!(tp->phy_flags & TG3_PHYFLG_ANY_SERDES))
		adv |= ADVERTISED_100baseT_Half |
		       ADVERTISED_100baseT_Full |
		       ADVERTISED_10baseT_Half |
		       ADVERTISED_10baseT_Full |
		       ADVERTISED_TP;
	else
		adv |= ADVERTISED_FIBRE;

	tp->link_config.advertising = adv;
	tp->link_config.speed = SPEED_UNKNOWN;
	tp->link_config.duplex = DUPLEX_UNKNOWN;
	tp->link_config.autoneg = AUTONEG_ENABLE;
	tp->link_config.active_speed = SPEED_UNKNOWN;
	tp->link_config.active_duplex = DUPLEX_UNKNOWN;

	tp->old_link = -1;
}
