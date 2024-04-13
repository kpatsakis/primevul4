static int atl2_request_irq(struct atl2_adapter *adapter)
{
	struct net_device *netdev = adapter->netdev;
	int flags, err = 0;

	flags = IRQF_SHARED;
	adapter->have_msi = true;
	err = pci_enable_msi(adapter->pdev);
	if (err)
		adapter->have_msi = false;

	if (adapter->have_msi)
		flags &= ~IRQF_SHARED;

	return request_irq(adapter->pdev->irq, atl2_intr, flags, netdev->name,
		netdev);
}
