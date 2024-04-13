static int atl2_sw_init(struct atl2_adapter *adapter)
{
	struct atl2_hw *hw = &adapter->hw;
	struct pci_dev *pdev = adapter->pdev;

	/* PCI config space info */
	hw->vendor_id = pdev->vendor;
	hw->device_id = pdev->device;
	hw->subsystem_vendor_id = pdev->subsystem_vendor;
	hw->subsystem_id = pdev->subsystem_device;
	hw->revision_id  = pdev->revision;

	pci_read_config_word(pdev, PCI_COMMAND, &hw->pci_cmd_word);

	adapter->wol = 0;
	adapter->ict = 50000;  /* ~100ms */
	adapter->link_speed = SPEED_0;   /* hardware init */
	adapter->link_duplex = FULL_DUPLEX;

	hw->phy_configured = false;
	hw->preamble_len = 7;
	hw->ipgt = 0x60;
	hw->min_ifg = 0x50;
	hw->ipgr1 = 0x40;
	hw->ipgr2 = 0x60;
	hw->retry_buf = 2;
	hw->max_retry = 0xf;
	hw->lcol = 0x37;
	hw->jam_ipg = 7;
	hw->fc_rxd_hi = 0;
	hw->fc_rxd_lo = 0;
	hw->max_frame_size = adapter->netdev->mtu;

	spin_lock_init(&adapter->stats_lock);

	set_bit(__ATL2_DOWN, &adapter->flags);

	return 0;
}
