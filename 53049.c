static void atl2_free_irq(struct atl2_adapter *adapter)
{
	struct net_device *netdev = adapter->netdev;

	free_irq(adapter->pdev->irq, netdev);

#ifdef CONFIG_PCI_MSI
	if (adapter->have_msi)
		pci_disable_msi(adapter->pdev);
#endif
}
