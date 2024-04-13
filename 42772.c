int call_netdevice_notifiers(unsigned long val, struct net_device *dev)
{
	ASSERT_RTNL();
	return raw_notifier_call_chain(&netdev_chain, val, dev);
}
