static pci_ers_result_t tg3_io_slot_reset(struct pci_dev *pdev)
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct tg3 *tp = netdev_priv(netdev);
	pci_ers_result_t rc = PCI_ERS_RESULT_DISCONNECT;
	int err;

	rtnl_lock();

	if (pci_enable_device(pdev)) {
		netdev_err(netdev, "Cannot re-enable PCI device after reset.\n");
		goto done;
	}

	pci_set_master(pdev);
	pci_restore_state(pdev);
	pci_save_state(pdev);

	if (!netif_running(netdev)) {
		rc = PCI_ERS_RESULT_RECOVERED;
		goto done;
	}

	err = tg3_power_up(tp);
	if (err)
		goto done;

	rc = PCI_ERS_RESULT_RECOVERED;

done:
	rtnl_unlock();

	return rc;
}
