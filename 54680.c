static int create_intf_ep_devs(struct usb_interface *intf)
{
	struct usb_device *udev = interface_to_usbdev(intf);
	struct usb_host_interface *alt = intf->cur_altsetting;
	int i;

	if (intf->ep_devs_created || intf->unregistering)
		return 0;

	for (i = 0; i < alt->desc.bNumEndpoints; ++i)
		(void) usb_create_ep_devs(&intf->dev, &alt->endpoint[i], udev);
	intf->ep_devs_created = 1;
	return 0;
}
