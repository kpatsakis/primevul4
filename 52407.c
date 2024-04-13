static void ims_pcu_irq(struct urb *urb)
{
	struct ims_pcu *pcu = urb->context;
	int retval, status;

	status = urb->status;

	switch (status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dev_dbg(pcu->dev, "%s - urb shutting down with status: %d\n",
			__func__, status);
		return;
	default:
		dev_dbg(pcu->dev, "%s - nonzero urb status received: %d\n",
			__func__, status);
		goto exit;
	}

	dev_dbg(pcu->dev, "%s: received %d: %*ph\n", __func__,
		urb->actual_length, urb->actual_length, pcu->urb_in_buf);

	if (urb == pcu->urb_in)
		ims_pcu_process_data(pcu, urb);

exit:
	retval = usb_submit_urb(urb, GFP_ATOMIC);
	if (retval && retval != -ENODEV)
		dev_err(pcu->dev, "%s - usb_submit_urb failed with result %d\n",
			__func__, retval);
}
