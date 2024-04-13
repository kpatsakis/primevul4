static void netdev_stats_to_stats64(struct rtnl_link_stats64 *stats64,
				    const struct net_device_stats *netdev_stats)
{
#if BITS_PER_LONG == 64
        BUILD_BUG_ON(sizeof(*stats64) != sizeof(*netdev_stats));
        memcpy(stats64, netdev_stats, sizeof(*stats64));
#else
	size_t i, n = sizeof(*stats64) / sizeof(u64);
	const unsigned long *src = (const unsigned long *)netdev_stats;
	u64 *dst = (u64 *)stats64;

	BUILD_BUG_ON(sizeof(*netdev_stats) / sizeof(unsigned long) !=
		     sizeof(*stats64) / sizeof(u64));
	for (i = 0; i < n; i++)
		dst[i] = src[i];
#endif
}
