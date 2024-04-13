static void usbhid_stop(struct hid_device *hid)
{
	struct usbhid_device *usbhid = hid->driver_data;

	if (WARN_ON(!usbhid))
		return;

	if (hid->quirks & HID_QUIRK_ALWAYS_POLL) {
		clear_bit(HID_IN_POLLING, &usbhid->iofl);
		usbhid->intf->needs_remote_wakeup = 0;
	}

	clear_bit(HID_STARTED, &usbhid->iofl);
	spin_lock_irq(&usbhid->lock);	/* Sync with error and led handlers */
	set_bit(HID_DISCONNECTED, &usbhid->iofl);
	spin_unlock_irq(&usbhid->lock);
	usb_kill_urb(usbhid->urbin);
	usb_kill_urb(usbhid->urbout);
	usb_kill_urb(usbhid->urbctrl);

	hid_cancel_delayed_stuff(usbhid);

	hid->claimed = 0;

	usb_free_urb(usbhid->urbin);
	usb_free_urb(usbhid->urbctrl);
	usb_free_urb(usbhid->urbout);
	usbhid->urbin = NULL; /* don't mess up next start */
	usbhid->urbctrl = NULL;
	usbhid->urbout = NULL;

	hid_free_buffers(hid_to_usb_dev(hid), hid);
}
