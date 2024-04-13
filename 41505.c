static int tg3_set_features(struct net_device *dev, netdev_features_t features)
{
	netdev_features_t changed = dev->features ^ features;

	if ((changed & NETIF_F_LOOPBACK) && netif_running(dev))
		tg3_set_loopback(dev, features);

	return 0;
}
