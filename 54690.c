void usb_deauthorize_interface(struct usb_interface *intf)
{
	struct device *dev = &intf->dev;

	device_lock(dev->parent);

	if (intf->authorized) {
		device_lock(dev);
		intf->authorized = 0;
		device_unlock(dev);

		usb_forced_unbind_intf(intf);
	}

	device_unlock(dev->parent);
}
