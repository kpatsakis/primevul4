static struct usb_device *usbdev_lookup_by_devt(dev_t devt)
{
	struct device *dev;

	dev = bus_find_device(&usb_bus_type, NULL,
			      (void *) (unsigned long) devt, match_devt);
	if (!dev)
		return NULL;
	return to_usb_device(dev);
}
