static inline u16 dev_cap_txqueue(struct net_device *dev, u16 queue_index)
{
	if (unlikely(queue_index >= dev->real_num_tx_queues)) {
		if (net_ratelimit()) {
			pr_warning("%s selects TX queue %d, but "
				"real number of TX queues is %d\n",
				dev->name, queue_index, dev->real_num_tx_queues);
		}
		return 0;
	}
	return queue_index;
}
