static int tg3_get_coalesce(struct net_device *dev, struct ethtool_coalesce *ec)
{
	struct tg3 *tp = netdev_priv(dev);

	memcpy(ec, &tp->coal, sizeof(*ec));
	return 0;
}
