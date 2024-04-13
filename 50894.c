static int hid_bus_match(struct device *dev, struct device_driver *drv)
{
	struct hid_driver *hdrv = to_hid_driver(drv);
	struct hid_device *hdev = to_hid_device(dev);

	return hid_match_device(hdev, hdrv) != NULL;
}
