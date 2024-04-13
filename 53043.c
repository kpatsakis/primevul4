static int atl2_close(struct net_device *netdev)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);

	WARN_ON(test_bit(__ATL2_RESETTING, &adapter->flags));

	atl2_down(adapter);
	atl2_free_irq(adapter);
	atl2_free_ring_resources(adapter);

	return 0;
}
