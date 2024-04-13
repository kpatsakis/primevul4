static void atl2_get_drvinfo(struct net_device *netdev,
	struct ethtool_drvinfo *drvinfo)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);

	strlcpy(drvinfo->driver,  atl2_driver_name, sizeof(drvinfo->driver));
	strlcpy(drvinfo->version, atl2_driver_version,
		sizeof(drvinfo->version));
	strlcpy(drvinfo->fw_version, "L2", sizeof(drvinfo->fw_version));
	strlcpy(drvinfo->bus_info, pci_name(adapter->pdev),
		sizeof(drvinfo->bus_info));
}
