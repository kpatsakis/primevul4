static void atl2_down(struct atl2_adapter *adapter)
{
	struct net_device *netdev = adapter->netdev;

	/* signal that we're down so the interrupt handler does not
	 * reschedule our watchdog timer */
	set_bit(__ATL2_DOWN, &adapter->flags);

	netif_tx_disable(netdev);

	/* reset MAC to disable all RX/TX */
	atl2_reset_hw(&adapter->hw);
	msleep(1);

	atl2_irq_disable(adapter);

	del_timer_sync(&adapter->watchdog_timer);
	del_timer_sync(&adapter->phy_config_timer);
	clear_bit(0, &adapter->cfg_phy);

	netif_carrier_off(netdev);
	adapter->link_speed = SPEED_0;
	adapter->link_duplex = -1;
}
