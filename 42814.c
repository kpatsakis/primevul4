void netdev_rx_handler_unregister(struct net_device *dev)
{

	ASSERT_RTNL();
	rcu_assign_pointer(dev->rx_handler, NULL);
	rcu_assign_pointer(dev->rx_handler_data, NULL);
}
