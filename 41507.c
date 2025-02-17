static void tg3_set_loopback(struct net_device *dev, netdev_features_t features)
{
	struct tg3 *tp = netdev_priv(dev);

	if (features & NETIF_F_LOOPBACK) {
		if (tp->mac_mode & MAC_MODE_PORT_INT_LPBACK)
			return;

		spin_lock_bh(&tp->lock);
		tg3_mac_loopback(tp, true);
		netif_carrier_on(tp->dev);
		spin_unlock_bh(&tp->lock);
		netdev_info(dev, "Internal MAC loopback mode enabled.\n");
	} else {
		if (!(tp->mac_mode & MAC_MODE_PORT_INT_LPBACK))
			return;

		spin_lock_bh(&tp->lock);
		tg3_mac_loopback(tp, false);
		/* Force link status check */
		tg3_setup_phy(tp, 1);
		spin_unlock_bh(&tp->lock);
		netdev_info(dev, "Internal MAC loopback mode disabled.\n");
	}
}
