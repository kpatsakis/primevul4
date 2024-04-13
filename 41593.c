static void wdm_int_callback(struct urb *urb)
{
	int rv = 0;
	int status = urb->status;
	struct wdm_device *desc;
	struct usb_cdc_notification *dr;

	desc = urb->context;
	dr = (struct usb_cdc_notification *)desc->sbuf;

	if (status) {
		switch (status) {
		case -ESHUTDOWN:
		case -ENOENT:
		case -ECONNRESET:
			return; /* unplug */
		case -EPIPE:
			set_bit(WDM_INT_STALL, &desc->flags);
			dev_err(&desc->intf->dev, "Stall on int endpoint\n");
			goto sw; /* halt is cleared in work */
		default:
			dev_err(&desc->intf->dev,
				"nonzero urb status received: %d\n", status);
			break;
		}
	}

	if (urb->actual_length < sizeof(struct usb_cdc_notification)) {
		dev_err(&desc->intf->dev, "wdm_int_callback - %d bytes\n",
			urb->actual_length);
		goto exit;
	}

	switch (dr->bNotificationType) {
	case USB_CDC_NOTIFY_RESPONSE_AVAILABLE:
		dev_dbg(&desc->intf->dev,
			"NOTIFY_RESPONSE_AVAILABLE received: index %d len %d",
			dr->wIndex, dr->wLength);
		break;

	case USB_CDC_NOTIFY_NETWORK_CONNECTION:

		dev_dbg(&desc->intf->dev,
			"NOTIFY_NETWORK_CONNECTION %s network",
			dr->wValue ? "connected to" : "disconnected from");
		goto exit;
	default:
		clear_bit(WDM_POLL_RUNNING, &desc->flags);
		dev_err(&desc->intf->dev,
			"unknown notification %d received: index %d len %d\n",
			dr->bNotificationType, dr->wIndex, dr->wLength);
		goto exit;
	}

	spin_lock(&desc->iuspin);
	clear_bit(WDM_READ, &desc->flags);
	set_bit(WDM_RESPONDING, &desc->flags);
	if (!test_bit(WDM_DISCONNECTING, &desc->flags)
		&& !test_bit(WDM_SUSPENDING, &desc->flags)) {
		rv = usb_submit_urb(desc->response, GFP_ATOMIC);
		dev_dbg(&desc->intf->dev, "%s: usb_submit_urb %d",
			__func__, rv);
	}
	spin_unlock(&desc->iuspin);
	if (rv < 0) {
		clear_bit(WDM_RESPONDING, &desc->flags);
		if (rv == -EPERM)
			return;
		if (rv == -ENOMEM) {
sw:
			rv = schedule_work(&desc->rxwork);
			if (rv)
				dev_err(&desc->intf->dev,
					"Cannot schedule work\n");
		}
	}
exit:
	rv = usb_submit_urb(urb, GFP_ATOMIC);
	if (rv)
		dev_err(&desc->intf->dev,
			"%s - usb_submit_urb failed with result %d\n",
			__func__, rv);

}
