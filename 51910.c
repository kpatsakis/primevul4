ppp_get_stats64(struct net_device *dev, struct rtnl_link_stats64 *stats64)
{
	struct ppp *ppp = netdev_priv(dev);

	ppp_recv_lock(ppp);
	stats64->rx_packets = ppp->stats64.rx_packets;
	stats64->rx_bytes   = ppp->stats64.rx_bytes;
	ppp_recv_unlock(ppp);

	ppp_xmit_lock(ppp);
	stats64->tx_packets = ppp->stats64.tx_packets;
	stats64->tx_bytes   = ppp->stats64.tx_bytes;
	ppp_xmit_unlock(ppp);

	stats64->rx_errors        = dev->stats.rx_errors;
	stats64->tx_errors        = dev->stats.tx_errors;
	stats64->rx_dropped       = dev->stats.rx_dropped;
	stats64->tx_dropped       = dev->stats.tx_dropped;
	stats64->rx_length_errors = dev->stats.rx_length_errors;

	return stats64;
}
