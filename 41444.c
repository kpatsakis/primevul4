static void tg3_phy_start(struct tg3 *tp)
{
	struct phy_device *phydev;

	if (!(tp->phy_flags & TG3_PHYFLG_IS_CONNECTED))
		return;

	phydev = tp->mdio_bus->phy_map[TG3_PHY_MII_ADDR];

	if (tp->phy_flags & TG3_PHYFLG_IS_LOW_POWER) {
		tp->phy_flags &= ~TG3_PHYFLG_IS_LOW_POWER;
		phydev->speed = tp->link_config.speed;
		phydev->duplex = tp->link_config.duplex;
		phydev->autoneg = tp->link_config.autoneg;
		phydev->advertising = tp->link_config.advertising;
	}

	phy_start(phydev);

	phy_start_aneg(phydev);
}
