static void hid_cease_io(struct usbhid_device *usbhid)
{
	del_timer_sync(&usbhid->io_retry);
	usb_kill_urb(usbhid->urbin);
	usb_kill_urb(usbhid->urbctrl);
	usb_kill_urb(usbhid->urbout);
}
