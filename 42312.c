static void tun_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	struct tun_struct *tun = netdev_priv(dev);

	strlcpy(info->driver, DRV_NAME, sizeof(info->driver));
	strlcpy(info->version, DRV_VERSION, sizeof(info->version));

	switch (tun->flags & TUN_TYPE_MASK) {
	case TUN_TUN_DEV:
		strlcpy(info->bus_info, "tun", sizeof(info->bus_info));
		break;
	case TUN_TAP_DEV:
		strlcpy(info->bus_info, "tap", sizeof(info->bus_info));
		break;
	}
}
