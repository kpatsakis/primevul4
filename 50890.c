int hid_add_device(struct hid_device *hdev)
{
	static atomic_t id = ATOMIC_INIT(0);
	int ret;

	if (WARN_ON(hdev->status & HID_STAT_ADDED))
		return -EBUSY;

	/* we need to kill them here, otherwise they will stay allocated to
	 * wait for coming driver */
	if (hid_ignore(hdev))
		return -ENODEV;

	/*
	 * Check for the mandatory transport channel.
	 */
	 if (!hdev->ll_driver->raw_request) {
		hid_err(hdev, "transport driver missing .raw_request()\n");
		return -EINVAL;
	 }

	/*
	 * Read the device report descriptor once and use as template
	 * for the driver-specific modifications.
	 */
	ret = hdev->ll_driver->parse(hdev);
	if (ret)
		return ret;
	if (!hdev->dev_rdesc)
		return -ENODEV;

	/*
	 * Scan generic devices for group information
	 */
	if (hid_ignore_special_drivers ||
	    (!hdev->group &&
	     !hid_match_id(hdev, hid_have_special_driver))) {
		ret = hid_scan_report(hdev);
		if (ret)
			hid_warn(hdev, "bad device descriptor (%d)\n", ret);
	}

	/* XXX hack, any other cleaner solution after the driver core
	 * is converted to allow more than 20 bytes as the device name? */
	dev_set_name(&hdev->dev, "%04X:%04X:%04X.%04X", hdev->bus,
		     hdev->vendor, hdev->product, atomic_inc_return(&id));

	hid_debug_register(hdev, dev_name(&hdev->dev));
	ret = device_add(&hdev->dev);
	if (!ret)
		hdev->status |= HID_STAT_ADDED;
	else
		hid_debug_unregister(hdev);

	return ret;
}
