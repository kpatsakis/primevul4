static int hid_reset_resume(struct usb_interface *intf)
{
	struct hid_device *hid = usb_get_intfdata(intf);
	int status;

	status = hid_post_reset(intf);
	if (status >= 0 && hid->driver && hid->driver->reset_resume) {
		int ret = hid->driver->reset_resume(hid);
		if (ret < 0)
			status = ret;
	}
	return status;
}
