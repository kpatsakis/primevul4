static void hid_reset(struct work_struct *work)
{
	struct usbhid_device *usbhid =
		container_of(work, struct usbhid_device, reset_work);
	struct hid_device *hid = usbhid->hid;
	int rc;

	if (test_bit(HID_CLEAR_HALT, &usbhid->iofl)) {
		dev_dbg(&usbhid->intf->dev, "clear halt\n");
		rc = usb_clear_halt(hid_to_usb_dev(hid), usbhid->urbin->pipe);
		clear_bit(HID_CLEAR_HALT, &usbhid->iofl);
		if (rc == 0) {
			hid_start_in(hid);
		} else {
			dev_dbg(&usbhid->intf->dev,
					"clear-halt failed: %d\n", rc);
			set_bit(HID_RESET_PENDING, &usbhid->iofl);
		}
	}

	if (test_bit(HID_RESET_PENDING, &usbhid->iofl)) {
		dev_dbg(&usbhid->intf->dev, "resetting device\n");
		usb_queue_reset_device(usbhid->intf);
	}
}
