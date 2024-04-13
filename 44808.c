static struct net_device_stats *wanxl_get_stats(struct net_device *dev)
{
	port_t *port = dev_to_port(dev);

	dev->stats.rx_over_errors = get_status(port)->rx_overruns;
	dev->stats.rx_frame_errors = get_status(port)->rx_frame_errors;
	dev->stats.rx_errors = dev->stats.rx_over_errors +
		dev->stats.rx_frame_errors;
	return &dev->stats;
}
