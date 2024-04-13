static int tg3_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct tg3 *tp = netdev_priv(dev);
	u32 speed = ethtool_cmd_speed(cmd);

	if (tg3_flag(tp, USE_PHYLIB)) {
		struct phy_device *phydev;
		if (!(tp->phy_flags & TG3_PHYFLG_IS_CONNECTED))
			return -EAGAIN;
		phydev = tp->mdio_bus->phy_map[TG3_PHY_MII_ADDR];
		return phy_ethtool_sset(phydev, cmd);
	}

	if (cmd->autoneg != AUTONEG_ENABLE &&
	    cmd->autoneg != AUTONEG_DISABLE)
		return -EINVAL;

	if (cmd->autoneg == AUTONEG_DISABLE &&
	    cmd->duplex != DUPLEX_FULL &&
	    cmd->duplex != DUPLEX_HALF)
		return -EINVAL;

	if (cmd->autoneg == AUTONEG_ENABLE) {
		u32 mask = ADVERTISED_Autoneg |
			   ADVERTISED_Pause |
			   ADVERTISED_Asym_Pause;

		if (!(tp->phy_flags & TG3_PHYFLG_10_100_ONLY))
			mask |= ADVERTISED_1000baseT_Half |
				ADVERTISED_1000baseT_Full;

		if (!(tp->phy_flags & TG3_PHYFLG_ANY_SERDES))
			mask |= ADVERTISED_100baseT_Half |
				ADVERTISED_100baseT_Full |
				ADVERTISED_10baseT_Half |
				ADVERTISED_10baseT_Full |
				ADVERTISED_TP;
		else
			mask |= ADVERTISED_FIBRE;

		if (cmd->advertising & ~mask)
			return -EINVAL;

		mask &= (ADVERTISED_1000baseT_Half |
			 ADVERTISED_1000baseT_Full |
			 ADVERTISED_100baseT_Half |
			 ADVERTISED_100baseT_Full |
			 ADVERTISED_10baseT_Half |
			 ADVERTISED_10baseT_Full);

		cmd->advertising &= mask;
	} else {
		if (tp->phy_flags & TG3_PHYFLG_ANY_SERDES) {
			if (speed != SPEED_1000)
				return -EINVAL;

			if (cmd->duplex != DUPLEX_FULL)
				return -EINVAL;
		} else {
			if (speed != SPEED_100 &&
			    speed != SPEED_10)
				return -EINVAL;
		}
	}

	tg3_full_lock(tp, 0);

	tp->link_config.autoneg = cmd->autoneg;
	if (cmd->autoneg == AUTONEG_ENABLE) {
		tp->link_config.advertising = (cmd->advertising |
					      ADVERTISED_Autoneg);
		tp->link_config.speed = SPEED_UNKNOWN;
		tp->link_config.duplex = DUPLEX_UNKNOWN;
	} else {
		tp->link_config.advertising = 0;
		tp->link_config.speed = speed;
		tp->link_config.duplex = cmd->duplex;
	}

	if (netif_running(dev))
		tg3_setup_phy(tp, 1);

	tg3_full_unlock(tp);

	return 0;
}
