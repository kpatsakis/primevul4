static int hid_resume(struct usb_interface *intf)
{
	struct hid_device *hid = usb_get_intfdata (intf);
	int status;

	status = hid_resume_common(hid, true);
	dev_dbg(&intf->dev, "resume status %d\n", status);
	return 0;
}
