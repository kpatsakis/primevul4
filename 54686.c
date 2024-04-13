void usb_authorize_interface(struct usb_interface *intf)
{
	struct device *dev = &intf->dev;

	if (!intf->authorized) {
		device_lock(dev);
		intf->authorized = 1; /* authorize interface */
		device_unlock(dev);
	}
}
