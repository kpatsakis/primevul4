static int hid_pre_reset(struct usb_interface *intf)
{
	struct hid_device *hid = usb_get_intfdata(intf);
	struct usbhid_device *usbhid = hid->driver_data;

	spin_lock_irq(&usbhid->lock);
	set_bit(HID_RESET_PENDING, &usbhid->iofl);
	spin_unlock_irq(&usbhid->lock);
	hid_cease_io(usbhid);

	return 0;
}
