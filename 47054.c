static void virtnet_get_ringparam(struct net_device *dev,
				struct ethtool_ringparam *ring)
{
	struct virtnet_info *vi = netdev_priv(dev);

	ring->rx_max_pending = virtqueue_get_vring_size(vi->rq[0].vq);
	ring->tx_max_pending = virtqueue_get_vring_size(vi->sq[0].vq);
	ring->rx_pending = ring->rx_max_pending;
	ring->tx_pending = ring->tx_max_pending;
}
