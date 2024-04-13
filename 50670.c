static void packet_cached_dev_assign(struct packet_sock *po,
				     struct net_device *dev)
{
	rcu_assign_pointer(po->cached_dev, dev);
}
