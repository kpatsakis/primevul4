struct usb_interface *usbhid_find_interface(int minor)
{
	return usb_find_interface(&hid_driver, minor);
}
