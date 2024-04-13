static int usbhid_idle(struct hid_device *hid, int report, int idle,
		int reqtype)
{
	struct usb_device *dev = hid_to_usb_dev(hid);
	struct usb_interface *intf = to_usb_interface(hid->dev.parent);
	struct usb_host_interface *interface = intf->cur_altsetting;
	int ifnum = interface->desc.bInterfaceNumber;

	if (reqtype != HID_REQ_SET_IDLE)
		return -EINVAL;

	return hid_set_idle(dev, ifnum, report, idle);
}
