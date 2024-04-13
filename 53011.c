static void powermate_irq(struct urb *urb)
{
	struct powermate_device *pm = urb->context;
	struct device *dev = &pm->intf->dev;
	int retval;

	switch (urb->status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dev_dbg(dev, "%s - urb shutting down with status: %d\n",
			__func__, urb->status);
		return;
	default:
		dev_dbg(dev, "%s - nonzero urb status received: %d\n",
			__func__, urb->status);
		goto exit;
	}

	/* handle updates to device state */
	input_report_key(pm->input, BTN_0, pm->data[0] & 0x01);
	input_report_rel(pm->input, REL_DIAL, pm->data[1]);
	input_sync(pm->input);

exit:
	retval = usb_submit_urb (urb, GFP_ATOMIC);
	if (retval)
		dev_err(dev, "%s - usb_submit_urb failed with result: %d\n",
			__func__, retval);
}
