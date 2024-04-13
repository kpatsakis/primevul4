static int get_altsetting(struct usbtest_dev *dev)
{
	struct usb_interface	*iface = dev->intf;
	struct usb_device	*udev = interface_to_usbdev(iface);
	int			retval;

	retval = usb_control_msg(udev, usb_rcvctrlpipe(udev, 0),
			USB_REQ_GET_INTERFACE, USB_DIR_IN|USB_RECIP_INTERFACE,
			0, iface->altsetting[0].desc.bInterfaceNumber,
			dev->buf, 1, USB_CTRL_GET_TIMEOUT);
	switch (retval) {
	case 1:
		return dev->buf[0];
	case 0:
		retval = -ERANGE;
		/* FALLTHROUGH */
	default:
		return retval;
	}
}
