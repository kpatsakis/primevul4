int netif_set_real_num_rx_queues(struct net_device *dev, unsigned int rxq)
{
	int rc;

	if (rxq < 1 || rxq > dev->num_rx_queues)
		return -EINVAL;

	if (dev->reg_state == NETREG_REGISTERED) {
		ASSERT_RTNL();

		rc = net_rx_queue_update_kobjects(dev, dev->real_num_rx_queues,
						  rxq);
		if (rc)
			return rc;
	}

	dev->real_num_rx_queues = rxq;
	return 0;
}
