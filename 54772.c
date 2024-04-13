static int set_altsetting(struct usbtest_dev *dev, int alternate)
{
	struct usb_interface		*iface = dev->intf;
	struct usb_device		*udev;

	if (alternate < 0 || alternate >= 256)
		return -EINVAL;

	udev = interface_to_usbdev(iface);
	return usb_set_interface(udev,
			iface->altsetting[0].desc.bInterfaceNumber,
			alternate);
}
