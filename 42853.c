static void ipgre_tap_setup(struct net_device *dev)
{

	ether_setup(dev);

	dev->netdev_ops		= &ipgre_tap_netdev_ops;
	dev->destructor 	= ipgre_dev_free;

	dev->iflink		= 0;
	dev->features		|= NETIF_F_NETNS_LOCAL;
}
