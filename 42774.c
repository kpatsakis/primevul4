static void __net_exit default_device_exit_batch(struct list_head *net_list)
{
	/* At exit all network devices most be removed from a network
	 * namespace.  Do this in the reverse order of registration.
	 * Do this across as many network namespaces as possible to
	 * improve batching efficiency.
	 */
	struct net_device *dev;
	struct net *net;
	LIST_HEAD(dev_kill_list);

	rtnl_lock();
	list_for_each_entry(net, net_list, exit_list) {
		for_each_netdev_reverse(net, dev) {
			if (dev->rtnl_link_ops)
				dev->rtnl_link_ops->dellink(dev, &dev_kill_list);
			else
				unregister_netdevice_queue(dev, &dev_kill_list);
		}
	}
	unregister_netdevice_many(&dev_kill_list);
	list_del(&dev_kill_list);
	rtnl_unlock();
}
