static int atl2_nway_reset(struct net_device *netdev)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);
	if (netif_running(netdev))
		atl2_reinit_locked(adapter);
	return 0;
}
