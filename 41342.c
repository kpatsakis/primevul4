static void tg3_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct tg3 *tp = netdev_priv(dev);

	strlcpy(info->driver, DRV_MODULE_NAME, sizeof(info->driver));
	strlcpy(info->version, DRV_MODULE_VERSION, sizeof(info->version));
	strlcpy(info->fw_version, tp->fw_ver, sizeof(info->fw_version));
	strlcpy(info->bus_info, pci_name(tp->pdev), sizeof(info->bus_info));
}
