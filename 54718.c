static void hid_ctrl(struct urb *urb)
{
	struct hid_device *hid = urb->context;
	struct usbhid_device *usbhid = hid->driver_data;
	int unplug = 0, status = urb->status;

	switch (status) {
	case 0:			/* success */
		if (usbhid->ctrl[usbhid->ctrltail].dir == USB_DIR_IN)
			hid_input_report(urb->context,
				usbhid->ctrl[usbhid->ctrltail].report->type,
				urb->transfer_buffer, urb->actual_length, 0);
		break;
	case -ESHUTDOWN:	/* unplug */
		unplug = 1;
	case -EILSEQ:		/* protocol error or unplug */
	case -EPROTO:		/* protocol error or unplug */
	case -ECONNRESET:	/* unlink */
	case -ENOENT:
	case -EPIPE:		/* report not available */
		break;
	default:		/* error */
		hid_warn(urb->dev, "ctrl urb status %d received\n", status);
	}

	spin_lock(&usbhid->lock);

	if (unplug) {
		usbhid->ctrltail = usbhid->ctrlhead;
	} else {
		usbhid->ctrltail = (usbhid->ctrltail + 1) & (HID_CONTROL_FIFO_SIZE - 1);

		if (usbhid->ctrlhead != usbhid->ctrltail &&
				hid_submit_ctrl(hid) == 0) {
			/* Successfully submitted next urb in queue */
			spin_unlock(&usbhid->lock);
			return;
		}
	}

	clear_bit(HID_CTRL_RUNNING, &usbhid->iofl);
	spin_unlock(&usbhid->lock);
	usb_autopm_put_interface_async(usbhid->intf);
	wake_up(&usbhid->wait);
}
