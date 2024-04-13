static int atl2_get_eeprom_len(struct net_device *netdev)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);

	if (!atl2_check_eeprom_exist(&adapter->hw))
		return 512;
	else
		return 0;
}
