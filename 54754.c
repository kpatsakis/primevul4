static int usbhid_wait_io(struct hid_device *hid)
{
	struct usbhid_device *usbhid = hid->driver_data;

	if (!wait_event_timeout(usbhid->wait,
				(!test_bit(HID_CTRL_RUNNING, &usbhid->iofl) &&
				!test_bit(HID_OUT_RUNNING, &usbhid->iofl)),
					10*HZ)) {
		dbg_hid("timeout waiting for ctrl or out queue to clear\n");
		return -1;
	}

	return 0;
}
