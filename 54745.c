static int usbhid_power(struct hid_device *hid, int lvl)
{
	struct usbhid_device *usbhid = hid->driver_data;
	int r = 0;

	switch (lvl) {
	case PM_HINT_FULLON:
		r = usb_autopm_get_interface(usbhid->intf);
		break;

	case PM_HINT_NORMAL:
		usb_autopm_put_interface(usbhid->intf);
		break;
	}

	return r;
}
