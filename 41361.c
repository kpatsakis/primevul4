static int tg3_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct tg3 *tp = netdev_priv(dev);

	if (tg3_flag(tp, USE_PHYLIB)) {
		struct phy_device *phydev;
		if (!(tp->phy_flags & TG3_PHYFLG_IS_CONNECTED))
			return -EAGAIN;
		phydev = tp->mdio_bus->phy_map[TG3_PHY_MII_ADDR];
		return phy_ethtool_gset(phydev, cmd);
	}

	cmd->supported = (SUPPORTED_Autoneg);

	if (!(tp->phy_flags & TG3_PHYFLG_10_100_ONLY))
		cmd->supported |= (SUPPORTED_1000baseT_Half |
				   SUPPORTED_1000baseT_Full);

	if (!(tp->phy_flags & TG3_PHYFLG_ANY_SERDES)) {
		cmd->supported |= (SUPPORTED_100baseT_Half |
				  SUPPORTED_100baseT_Full |
				  SUPPORTED_10baseT_Half |
				  SUPPORTED_10baseT_Full |
				  SUPPORTED_TP);
		cmd->port = PORT_TP;
	} else {
		cmd->supported |= SUPPORTED_FIBRE;
		cmd->port = PORT_FIBRE;
	}

	cmd->advertising = tp->link_config.advertising;
	if (tg3_flag(tp, PAUSE_AUTONEG)) {
		if (tp->link_config.flowctrl & FLOW_CTRL_RX) {
			if (tp->link_config.flowctrl & FLOW_CTRL_TX) {
				cmd->advertising |= ADVERTISED_Pause;
			} else {
				cmd->advertising |= ADVERTISED_Pause |
						    ADVERTISED_Asym_Pause;
			}
		} else if (tp->link_config.flowctrl & FLOW_CTRL_TX) {
			cmd->advertising |= ADVERTISED_Asym_Pause;
		}
	}
	if (netif_running(dev) && tp->link_up) {
		ethtool_cmd_speed_set(cmd, tp->link_config.active_speed);
		cmd->duplex = tp->link_config.active_duplex;
		cmd->lp_advertising = tp->link_config.rmt_adv;
		if (!(tp->phy_flags & TG3_PHYFLG_ANY_SERDES)) {
			if (tp->phy_flags & TG3_PHYFLG_MDIX_STATE)
				cmd->eth_tp_mdix = ETH_TP_MDI_X;
			else
				cmd->eth_tp_mdix = ETH_TP_MDI;
		}
	} else {
		ethtool_cmd_speed_set(cmd, SPEED_UNKNOWN);
		cmd->duplex = DUPLEX_UNKNOWN;
		cmd->eth_tp_mdix = ETH_TP_MDI_INVALID;
	}
	cmd->phy_address = tp->phy_addr;
	cmd->transceiver = XCVR_INTERNAL;
	cmd->autoneg = tp->link_config.autoneg;
	cmd->maxtxpkt = 0;
	cmd->maxrxpkt = 0;
	return 0;
}
