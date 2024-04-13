static void atl2_remove(struct pci_dev *pdev)
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct atl2_adapter *adapter = netdev_priv(netdev);

	/* flush_scheduled work may reschedule our watchdog task, so
	 * explicitly disable watchdog tasks from being rescheduled  */
	set_bit(__ATL2_DOWN, &adapter->flags);

	del_timer_sync(&adapter->watchdog_timer);
	del_timer_sync(&adapter->phy_config_timer);
	cancel_work_sync(&adapter->reset_task);
	cancel_work_sync(&adapter->link_chg_task);

	unregister_netdev(netdev);

	atl2_force_ps(&adapter->hw);

	iounmap(adapter->hw.hw_addr);
	pci_release_regions(pdev);

	free_netdev(netdev);

	pci_disable_device(pdev);
}
