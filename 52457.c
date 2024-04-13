static void acm_ctrl_irq(struct urb *urb)
{
	struct acm *acm = urb->context;
	struct usb_cdc_notification *dr = urb->transfer_buffer;
	unsigned char *data;
	int newctrl;
	int difference;
	int retval;
	int status = urb->status;

	switch (status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dev_dbg(&acm->control->dev,
				"%s - urb shutting down with status: %d\n",
				__func__, status);
		return;
	default:
		dev_dbg(&acm->control->dev,
				"%s - nonzero urb status received: %d\n",
				__func__, status);
		goto exit;
	}

	usb_mark_last_busy(acm->dev);

	data = (unsigned char *)(dr + 1);
	switch (dr->bNotificationType) {
	case USB_CDC_NOTIFY_NETWORK_CONNECTION:
		dev_dbg(&acm->control->dev, "%s - network connection: %d\n",
							__func__, dr->wValue);
		break;

	case USB_CDC_NOTIFY_SERIAL_STATE:
		newctrl = get_unaligned_le16(data);

		if (!acm->clocal && (acm->ctrlin & ~newctrl & ACM_CTRL_DCD)) {
			dev_dbg(&acm->control->dev, "%s - calling hangup\n",
					__func__);
			tty_port_tty_hangup(&acm->port, false);
		}

		difference = acm->ctrlin ^ newctrl;
		spin_lock(&acm->read_lock);
		acm->ctrlin = newctrl;
		acm->oldcount = acm->iocount;

		if (difference & ACM_CTRL_DSR)
			acm->iocount.dsr++;
		if (difference & ACM_CTRL_BRK)
			acm->iocount.brk++;
		if (difference & ACM_CTRL_RI)
			acm->iocount.rng++;
		if (difference & ACM_CTRL_DCD)
			acm->iocount.dcd++;
		if (difference & ACM_CTRL_FRAMING)
			acm->iocount.frame++;
		if (difference & ACM_CTRL_PARITY)
			acm->iocount.parity++;
		if (difference & ACM_CTRL_OVERRUN)
			acm->iocount.overrun++;
		spin_unlock(&acm->read_lock);

		if (difference)
			wake_up_all(&acm->wioctl);

		break;

	default:
		dev_dbg(&acm->control->dev,
			"%s - unknown notification %d received: index %d "
			"len %d data0 %d data1 %d\n",
			__func__,
			dr->bNotificationType, dr->wIndex,
			dr->wLength, data[0], data[1]);
		break;
	}
exit:
	retval = usb_submit_urb(urb, GFP_ATOMIC);
	if (retval && retval != -EPERM)
		dev_err(&acm->control->dev, "%s - usb_submit_urb failed: %d\n",
							__func__, retval);
}
