static int ap_bus_match(struct device *dev, struct device_driver *drv)
{
	struct ap_device *ap_dev = to_ap_dev(dev);
	struct ap_driver *ap_drv = to_ap_drv(drv);
	struct ap_device_id *id;

	/*
	 * Compare device type of the device with the list of
	 * supported types of the device_driver.
	 */
	for (id = ap_drv->ids; id->match_flags; id++) {
		if ((id->match_flags & AP_DEVICE_ID_MATCH_DEVICE_TYPE) &&
		    (id->dev_type != ap_dev->device_type))
			continue;
		return 1;
	}
	return 0;
}
