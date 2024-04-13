struct net_device *dev_get_by_flags_rcu(struct net *net, unsigned short if_flags,
				    unsigned short mask)
{
	struct net_device *dev, *ret;

	ret = NULL;
	for_each_netdev_rcu(net, dev) {
		if (((dev->flags ^ if_flags) & mask) == 0) {
			ret = dev;
			break;
		}
	}
	return ret;
}
