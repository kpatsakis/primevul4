static int catc_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct catc *catc = netdev_priv(dev);
	if (!catc->is_f5u011)
		return -EOPNOTSUPP;

	cmd->supported = SUPPORTED_10baseT_Half | SUPPORTED_TP;
	cmd->advertising = ADVERTISED_10baseT_Half | ADVERTISED_TP;
	ethtool_cmd_speed_set(cmd, SPEED_10);
	cmd->duplex = DUPLEX_HALF;
	cmd->port = PORT_TP; 
	cmd->phy_address = 0;
	cmd->transceiver = XCVR_INTERNAL;
	cmd->autoneg = AUTONEG_DISABLE;
	cmd->maxtxpkt = 1;
	cmd->maxrxpkt = 1;
	return 0;
}
