static void netdev_init_one_queue(struct net_device *dev,
				  struct netdev_queue *queue, void *_unused)
{
	/* Initialize queue lock */
	spin_lock_init(&queue->_xmit_lock);
	netdev_set_xmit_lockdep_class(&queue->_xmit_lock, dev->type);
	queue->xmit_lock_owner = -1;
	netdev_queue_numa_node_write(queue, NUMA_NO_NODE);
	queue->dev = dev;
}
