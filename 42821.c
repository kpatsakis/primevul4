int netif_set_real_num_tx_queues(struct net_device *dev, unsigned int txq)
{
	int rc;

	if (txq < 1 || txq > dev->num_tx_queues)
		return -EINVAL;

	if (dev->reg_state == NETREG_REGISTERED) {
		ASSERT_RTNL();

		rc = netdev_queue_update_kobjects(dev, dev->real_num_tx_queues,
						  txq);
		if (rc)
			return rc;

		if (txq < dev->real_num_tx_queues)
			qdisc_reset_all_tx_gt(dev, txq);
	}

	dev->real_num_tx_queues = txq;
	return 0;
}
