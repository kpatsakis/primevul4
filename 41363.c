static struct rtnl_link_stats64 *tg3_get_stats64(struct net_device *dev,
						struct rtnl_link_stats64 *stats)
{
	struct tg3 *tp = netdev_priv(dev);

	spin_lock_bh(&tp->lock);
	if (!tp->hw_stats) {
		spin_unlock_bh(&tp->lock);
		return &tp->net_stats_prev;
	}

	tg3_get_nstats(tp, stats);
	spin_unlock_bh(&tp->lock);

	return stats;
}
