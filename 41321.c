static netdev_features_t tg3_fix_features(struct net_device *dev,
	netdev_features_t features)
{
	struct tg3 *tp = netdev_priv(dev);

	if (dev->mtu > ETH_DATA_LEN && tg3_flag(tp, 5780_CLASS))
		features &= ~NETIF_F_ALL_TSO;

	return features;
}
