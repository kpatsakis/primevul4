struct netdev_queue *dev_ingress_queue_create(struct net_device *dev)
{
	struct netdev_queue *queue = dev_ingress_queue(dev);

#ifdef CONFIG_NET_CLS_ACT
	if (queue)
		return queue;
	queue = kzalloc(sizeof(*queue), GFP_KERNEL);
	if (!queue)
		return NULL;
	netdev_init_one_queue(dev, queue, NULL);
	queue->qdisc = &noop_qdisc;
	queue->qdisc_sleeping = &noop_qdisc;
	rcu_assign_pointer(dev->ingress_queue, queue);
#endif
	return queue;
}
