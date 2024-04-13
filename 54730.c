static void hid_restart_io(struct hid_device *hid)
{
	struct usbhid_device *usbhid = hid->driver_data;
	int clear_halt = test_bit(HID_CLEAR_HALT, &usbhid->iofl);
	int reset_pending = test_bit(HID_RESET_PENDING, &usbhid->iofl);

	spin_lock_irq(&usbhid->lock);
	clear_bit(HID_SUSPENDED, &usbhid->iofl);
	usbhid_mark_busy(usbhid);

	if (clear_halt || reset_pending)
		schedule_work(&usbhid->reset_work);
	usbhid->retry_delay = 0;
	spin_unlock_irq(&usbhid->lock);

	if (reset_pending || !test_bit(HID_STARTED, &usbhid->iofl))
		return;

	if (!clear_halt) {
		if (hid_start_in(hid) < 0)
			hid_io_error(hid);
	}

	spin_lock_irq(&usbhid->lock);
	if (usbhid->urbout && !test_bit(HID_OUT_RUNNING, &usbhid->iofl))
		usbhid_restart_out_queue(usbhid);
	if (!test_bit(HID_CTRL_RUNNING, &usbhid->iofl))
		usbhid_restart_ctrl_queue(usbhid);
	spin_unlock_irq(&usbhid->lock);
}
