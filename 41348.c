static void tg3_get_ethtool_stats(struct net_device *dev,
				   struct ethtool_stats *estats, u64 *tmp_stats)
{
	struct tg3 *tp = netdev_priv(dev);

	if (tp->hw_stats)
		tg3_get_estats(tp, (struct tg3_ethtool_stats *)tmp_stats);
	else
		memset(tmp_stats, 0, sizeof(struct tg3_ethtool_stats));
}
