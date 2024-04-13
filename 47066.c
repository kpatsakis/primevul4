static struct rtnl_link_stats64 *virtnet_stats(struct net_device *dev,
					       struct rtnl_link_stats64 *tot)
{
	struct virtnet_info *vi = netdev_priv(dev);
	int cpu;
	unsigned int start;

	for_each_possible_cpu(cpu) {
		struct virtnet_stats *stats = per_cpu_ptr(vi->stats, cpu);
		u64 tpackets, tbytes, rpackets, rbytes;

		do {
			start = u64_stats_fetch_begin_irq(&stats->tx_syncp);
			tpackets = stats->tx_packets;
			tbytes   = stats->tx_bytes;
		} while (u64_stats_fetch_retry_irq(&stats->tx_syncp, start));

		do {
			start = u64_stats_fetch_begin_irq(&stats->rx_syncp);
			rpackets = stats->rx_packets;
			rbytes   = stats->rx_bytes;
		} while (u64_stats_fetch_retry_irq(&stats->rx_syncp, start));

		tot->rx_packets += rpackets;
		tot->tx_packets += tpackets;
		tot->rx_bytes   += rbytes;
		tot->tx_bytes   += tbytes;
	}

	tot->tx_dropped = dev->stats.tx_dropped;
	tot->tx_fifo_errors = dev->stats.tx_fifo_errors;
	tot->rx_dropped = dev->stats.rx_dropped;
	tot->rx_length_errors = dev->stats.rx_length_errors;
	tot->rx_frame_errors = dev->stats.rx_frame_errors;

	return tot;
}
