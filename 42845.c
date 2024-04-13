static struct net_device_stats *ipgre_get_stats(struct net_device *dev)
{
	struct pcpu_tstats sum = { 0 };
	int i;

	for_each_possible_cpu(i) {
		const struct pcpu_tstats *tstats = per_cpu_ptr(dev->tstats, i);

		sum.rx_packets += tstats->rx_packets;
		sum.rx_bytes   += tstats->rx_bytes;
		sum.tx_packets += tstats->tx_packets;
		sum.tx_bytes   += tstats->tx_bytes;
	}
	dev->stats.rx_packets = sum.rx_packets;
	dev->stats.rx_bytes   = sum.rx_bytes;
	dev->stats.tx_packets = sum.tx_packets;
	dev->stats.tx_bytes   = sum.tx_bytes;
	return &dev->stats;
}
