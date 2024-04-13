static int netif_alloc_netdev_queues(struct net_device *dev)
{
	unsigned int count = dev->num_tx_queues;
	struct netdev_queue *tx;

	BUG_ON(count < 1);

	tx = kcalloc(count, sizeof(struct netdev_queue), GFP_KERNEL);
	if (!tx) {
		pr_err("netdev: Unable to allocate %u tx queues.\n",
		       count);
		return -ENOMEM;
	}
	dev->_tx = tx;

	netdev_for_each_tx_queue(dev, netdev_init_one_queue, NULL);
	spin_lock_init(&dev->tx_global_lock);

	return 0;
}
