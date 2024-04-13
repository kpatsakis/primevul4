static void tg3_get_nstats(struct tg3 *tp, struct rtnl_link_stats64 *stats)
{
	struct rtnl_link_stats64 *old_stats = &tp->net_stats_prev;
	struct tg3_hw_stats *hw_stats = tp->hw_stats;

	stats->rx_packets = old_stats->rx_packets +
		get_stat64(&hw_stats->rx_ucast_packets) +
		get_stat64(&hw_stats->rx_mcast_packets) +
		get_stat64(&hw_stats->rx_bcast_packets);

	stats->tx_packets = old_stats->tx_packets +
		get_stat64(&hw_stats->tx_ucast_packets) +
		get_stat64(&hw_stats->tx_mcast_packets) +
		get_stat64(&hw_stats->tx_bcast_packets);

	stats->rx_bytes = old_stats->rx_bytes +
		get_stat64(&hw_stats->rx_octets);
	stats->tx_bytes = old_stats->tx_bytes +
		get_stat64(&hw_stats->tx_octets);

	stats->rx_errors = old_stats->rx_errors +
		get_stat64(&hw_stats->rx_errors);
	stats->tx_errors = old_stats->tx_errors +
		get_stat64(&hw_stats->tx_errors) +
		get_stat64(&hw_stats->tx_mac_errors) +
		get_stat64(&hw_stats->tx_carrier_sense_errors) +
		get_stat64(&hw_stats->tx_discards);

	stats->multicast = old_stats->multicast +
		get_stat64(&hw_stats->rx_mcast_packets);
	stats->collisions = old_stats->collisions +
		get_stat64(&hw_stats->tx_collisions);

	stats->rx_length_errors = old_stats->rx_length_errors +
		get_stat64(&hw_stats->rx_frame_too_long_errors) +
		get_stat64(&hw_stats->rx_undersize_packets);

	stats->rx_over_errors = old_stats->rx_over_errors +
		get_stat64(&hw_stats->rxbds_empty);
	stats->rx_frame_errors = old_stats->rx_frame_errors +
		get_stat64(&hw_stats->rx_align_errors);
	stats->tx_aborted_errors = old_stats->tx_aborted_errors +
		get_stat64(&hw_stats->tx_discards);
	stats->tx_carrier_errors = old_stats->tx_carrier_errors +
		get_stat64(&hw_stats->tx_carrier_sense_errors);

	stats->rx_crc_errors = old_stats->rx_crc_errors +
		tg3_calc_crc_errors(tp);

	stats->rx_missed_errors = old_stats->rx_missed_errors +
		get_stat64(&hw_stats->rx_discards);

	stats->rx_dropped = tp->rx_dropped;
	stats->tx_dropped = tp->tx_dropped;
}
