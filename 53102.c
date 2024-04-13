static void atl2_tx_timeout(struct net_device *netdev)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);

	/* Do the reset outside of interrupt context */
	schedule_work(&adapter->reset_task);
}
