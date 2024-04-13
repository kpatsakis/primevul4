static int choose_config(struct usb_device *dev)
{
	/*
	 * There may be multiple configurations on this device, in which case
	 * we would need to read and parse all of them to find out which one
	 * we want. However, we just support one config at this point,
	 * configuration # 1, which is Config Descriptor 0.
	 */

	dev_dbg(&dev->dev, "%s - Number of Interfaces = %d\n",
		__func__, dev->config->desc.bNumInterfaces);
	dev_dbg(&dev->dev, "%s - MAX Power            = %d\n",
		__func__, dev->config->desc.bMaxPower * 2);

	if (dev->config->desc.bNumInterfaces != 1) {
		dev_err(&dev->dev, "%s - bNumInterfaces is not 1, ERROR!\n", __func__);
		return -ENODEV;
	}

	return 0;
}
