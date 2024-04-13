static void usbhid_mark_busy(struct usbhid_device *usbhid)
{
	struct usb_interface *intf = usbhid->intf;

	usb_mark_last_busy(interface_to_usbdev(intf));
}
