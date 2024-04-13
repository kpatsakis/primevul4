static int hid_suspend(struct usb_interface *intf, pm_message_t message)
{
	struct hid_device *hid = usb_get_intfdata(intf);
	struct usbhid_device *usbhid = hid->driver_data;
	int status = 0;
	bool driver_suspended = false;
	unsigned int ledcount;

	if (PMSG_IS_AUTO(message)) {
		ledcount = hidinput_count_leds(hid);
		spin_lock_irq(&usbhid->lock);	/* Sync with error handler */
		if (!test_bit(HID_RESET_PENDING, &usbhid->iofl)
		    && !test_bit(HID_CLEAR_HALT, &usbhid->iofl)
		    && !test_bit(HID_OUT_RUNNING, &usbhid->iofl)
		    && !test_bit(HID_CTRL_RUNNING, &usbhid->iofl)
		    && !test_bit(HID_KEYS_PRESSED, &usbhid->iofl)
		    && (!ledcount || ignoreled))
		{
			set_bit(HID_SUSPENDED, &usbhid->iofl);
			spin_unlock_irq(&usbhid->lock);
			if (hid->driver && hid->driver->suspend) {
				status = hid->driver->suspend(hid, message);
				if (status < 0)
					goto failed;
			}
			driver_suspended = true;
		} else {
			usbhid_mark_busy(usbhid);
			spin_unlock_irq(&usbhid->lock);
			return -EBUSY;
		}

	} else {
		/* TODO: resume() might need to handle suspend failure */
		if (hid->driver && hid->driver->suspend)
			status = hid->driver->suspend(hid, message);
		driver_suspended = true;
		spin_lock_irq(&usbhid->lock);
		set_bit(HID_SUSPENDED, &usbhid->iofl);
		spin_unlock_irq(&usbhid->lock);
		if (usbhid_wait_io(hid) < 0)
			status = -EIO;
	}

	hid_cancel_delayed_stuff(usbhid);
	hid_cease_io(usbhid);

	if (PMSG_IS_AUTO(message) && test_bit(HID_KEYS_PRESSED, &usbhid->iofl)) {
		/* lost race against keypresses */
		status = -EBUSY;
		goto failed;
	}
	dev_dbg(&intf->dev, "suspend\n");
	return status;

 failed:
	hid_resume_common(hid, driver_suspended);
	return status;
}
