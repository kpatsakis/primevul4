static void catc_get_drvinfo(struct net_device *dev,
			     struct ethtool_drvinfo *info)
{
	struct catc *catc = netdev_priv(dev);
	strlcpy(info->driver, driver_name, sizeof(info->driver));
	strlcpy(info->version, DRIVER_VERSION, sizeof(info->version));
	usb_make_path(catc->usbdev, info->bus_info, sizeof(info->bus_info));
}
