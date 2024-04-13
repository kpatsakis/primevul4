static void hid_retry_timeout(unsigned long _hid)
{
	struct hid_device *hid = (struct hid_device *) _hid;
	struct usbhid_device *usbhid = hid->driver_data;

	dev_dbg(&usbhid->intf->dev, "retrying intr urb\n");
	if (hid_start_in(hid))
		hid_io_error(hid);
}
