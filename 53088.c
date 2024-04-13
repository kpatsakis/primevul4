static int atl2_resume(struct pci_dev *pdev)
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct atl2_adapter *adapter = netdev_priv(netdev);
	u32 err;

	pci_set_power_state(pdev, PCI_D0);
	pci_restore_state(pdev);

	err = pci_enable_device(pdev);
	if (err) {
		printk(KERN_ERR
			"atl2: Cannot enable PCI device from suspend\n");
		return err;
	}

	pci_set_master(pdev);

	ATL2_READ_REG(&adapter->hw, REG_WOL_CTRL); /* clear WOL status */

	pci_enable_wake(pdev, PCI_D3hot, 0);
	pci_enable_wake(pdev, PCI_D3cold, 0);

	ATL2_WRITE_REG(&adapter->hw, REG_WOL_CTRL, 0);

	if (netif_running(netdev)) {
		err = atl2_request_irq(adapter);
		if (err)
			return err;
	}

	atl2_reset_hw(&adapter->hw);

	if (netif_running(netdev))
		atl2_up(adapter);

	netif_device_attach(netdev);

	return 0;
}
