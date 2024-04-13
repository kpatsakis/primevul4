static int rtnl_fdb_dump(struct sk_buff *skb, struct netlink_callback *cb)
{
	int idx = 0;
	struct net *net = sock_net(skb->sk);
	struct net_device *dev;

	rcu_read_lock();
	for_each_netdev_rcu(net, dev) {
		if (dev->priv_flags & IFF_BRIDGE_PORT) {
			struct net_device *br_dev;
			const struct net_device_ops *ops;

			br_dev = netdev_master_upper_dev_get(dev);
			ops = br_dev->netdev_ops;
			if (ops->ndo_fdb_dump)
				idx = ops->ndo_fdb_dump(skb, cb, dev, idx);
		}

		if (dev->netdev_ops->ndo_fdb_dump)
			idx = dev->netdev_ops->ndo_fdb_dump(skb, cb, dev, idx);
	}
	rcu_read_unlock();

	cb->args[0] = idx;
	return skb->len;
}
