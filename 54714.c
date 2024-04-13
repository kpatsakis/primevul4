static void __usbhid_submit_report(struct hid_device *hid, struct hid_report *report,
				   unsigned char dir)
{
	int head;
	struct usbhid_device *usbhid = hid->driver_data;

	if (((hid->quirks & HID_QUIRK_NOGET) && dir == USB_DIR_IN) ||
		test_bit(HID_DISCONNECTED, &usbhid->iofl))
		return;

	if (usbhid->urbout && dir == USB_DIR_OUT && report->type == HID_OUTPUT_REPORT) {
		if ((head = (usbhid->outhead + 1) & (HID_OUTPUT_FIFO_SIZE - 1)) == usbhid->outtail) {
			hid_warn(hid, "output queue full\n");
			return;
		}

		usbhid->out[usbhid->outhead].raw_report = hid_alloc_report_buf(report, GFP_ATOMIC);
		if (!usbhid->out[usbhid->outhead].raw_report) {
			hid_warn(hid, "output queueing failed\n");
			return;
		}
		hid_output_report(report, usbhid->out[usbhid->outhead].raw_report);
		usbhid->out[usbhid->outhead].report = report;
		usbhid->outhead = head;

		/* If the queue isn't running, restart it */
		if (!test_bit(HID_OUT_RUNNING, &usbhid->iofl)) {
			usbhid_restart_out_queue(usbhid);

		/* Otherwise see if an earlier request has timed out */
		} else if (time_after(jiffies, usbhid->last_out + HZ * 5)) {

			/* Prevent autosuspend following the unlink */
			usb_autopm_get_interface_no_resume(usbhid->intf);

			/*
			 * Prevent resubmission in case the URB completes
			 * before we can unlink it.  We don't want to cancel
			 * the wrong transfer!
			 */
			usb_block_urb(usbhid->urbout);

			/* Drop lock to avoid deadlock if the callback runs */
			spin_unlock(&usbhid->lock);

			usb_unlink_urb(usbhid->urbout);
			spin_lock(&usbhid->lock);
			usb_unblock_urb(usbhid->urbout);

			/* Unlink might have stopped the queue */
			if (!test_bit(HID_OUT_RUNNING, &usbhid->iofl))
				usbhid_restart_out_queue(usbhid);

			/* Now we can allow autosuspend again */
			usb_autopm_put_interface_async(usbhid->intf);
		}
		return;
	}

	if ((head = (usbhid->ctrlhead + 1) & (HID_CONTROL_FIFO_SIZE - 1)) == usbhid->ctrltail) {
		hid_warn(hid, "control queue full\n");
		return;
	}

	if (dir == USB_DIR_OUT) {
		usbhid->ctrl[usbhid->ctrlhead].raw_report = hid_alloc_report_buf(report, GFP_ATOMIC);
		if (!usbhid->ctrl[usbhid->ctrlhead].raw_report) {
			hid_warn(hid, "control queueing failed\n");
			return;
		}
		hid_output_report(report, usbhid->ctrl[usbhid->ctrlhead].raw_report);
	}
	usbhid->ctrl[usbhid->ctrlhead].report = report;
	usbhid->ctrl[usbhid->ctrlhead].dir = dir;
	usbhid->ctrlhead = head;

	/* If the queue isn't running, restart it */
	if (!test_bit(HID_CTRL_RUNNING, &usbhid->iofl)) {
		usbhid_restart_ctrl_queue(usbhid);

	/* Otherwise see if an earlier request has timed out */
	} else if (time_after(jiffies, usbhid->last_ctrl + HZ * 5)) {

		/* Prevent autosuspend following the unlink */
		usb_autopm_get_interface_no_resume(usbhid->intf);

		/*
		 * Prevent resubmission in case the URB completes
		 * before we can unlink it.  We don't want to cancel
		 * the wrong transfer!
		 */
		usb_block_urb(usbhid->urbctrl);

		/* Drop lock to avoid deadlock if the callback runs */
		spin_unlock(&usbhid->lock);

		usb_unlink_urb(usbhid->urbctrl);
		spin_lock(&usbhid->lock);
		usb_unblock_urb(usbhid->urbctrl);

		/* Unlink might have stopped the queue */
		if (!test_bit(HID_CTRL_RUNNING, &usbhid->iofl))
			usbhid_restart_ctrl_queue(usbhid);

		/* Now we can allow autosuspend again */
		usb_autopm_put_interface_async(usbhid->intf);
	}
}
