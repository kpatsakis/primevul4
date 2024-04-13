static void atl2_check_for_link(struct atl2_adapter *adapter)
{
	struct net_device *netdev = adapter->netdev;
	u16 phy_data = 0;

	spin_lock(&adapter->stats_lock);
	atl2_read_phy_reg(&adapter->hw, MII_BMSR, &phy_data);
	atl2_read_phy_reg(&adapter->hw, MII_BMSR, &phy_data);
	spin_unlock(&adapter->stats_lock);

	/* notify upper layer link down ASAP */
	if (!(phy_data & BMSR_LSTATUS)) { /* Link Down */
		if (netif_carrier_ok(netdev)) { /* old link state: Up */
		printk(KERN_INFO "%s: %s NIC Link is Down\n",
			atl2_driver_name, netdev->name);
		adapter->link_speed = SPEED_0;
		netif_carrier_off(netdev);
		netif_stop_queue(netdev);
		}
	}
	schedule_work(&adapter->link_chg_task);
}
