static int usb_get_report(struct usb_device *dev,
			  struct usb_host_interface *inter, unsigned char type,
			  unsigned char id, void *buf, int size)
{
	return usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
			       USB_REQ_GET_REPORT,
			       USB_DIR_IN | USB_TYPE_CLASS |
			       USB_RECIP_INTERFACE, (type << 8) + id,
			       inter->desc.bInterfaceNumber, buf, size,
			       GET_TIMEOUT*HZ);
}
