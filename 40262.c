static void __rtnl_kill_links(struct net *net, struct rtnl_link_ops *ops)
{
	struct net_device *dev;
	LIST_HEAD(list_kill);

	for_each_netdev(net, dev) {
		if (dev->rtnl_link_ops == ops)
			ops->dellink(dev, &list_kill);
	}
	unregister_netdevice_many(&list_kill);
}
