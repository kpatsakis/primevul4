static void acm_read_bulk_callback(struct urb *urb)
{
	struct acm_rb *rb = urb->context;
	struct acm *acm = rb->instance;
	unsigned long flags;
	int status = urb->status;

	dev_vdbg(&acm->data->dev, "%s - urb %d, len %d\n", __func__,
					rb->index, urb->actual_length);

	if (!acm->dev) {
		set_bit(rb->index, &acm->read_urbs_free);
		dev_dbg(&acm->data->dev, "%s - disconnected\n", __func__);
		return;
	}

	if (status) {
		set_bit(rb->index, &acm->read_urbs_free);
		dev_dbg(&acm->data->dev, "%s - non-zero urb status: %d\n",
							__func__, status);
		if ((status != -ENOENT) || (urb->actual_length == 0))
			return;
	}

	usb_mark_last_busy(acm->dev);

	acm_process_read_urb(acm, urb);
	/*
	 * Unthrottle may run on another CPU which needs to see events
	 * in the same order. Submission has an implict barrier
	 */
	smp_mb__before_atomic();
	set_bit(rb->index, &acm->read_urbs_free);

	/* throttle device if requested by tty */
	spin_lock_irqsave(&acm->read_lock, flags);
	acm->throttled = acm->throttle_req;
	if (!acm->throttled) {
		spin_unlock_irqrestore(&acm->read_lock, flags);
		acm_submit_read_urb(acm, rb->index, GFP_ATOMIC);
	} else {
		spin_unlock_irqrestore(&acm->read_lock, flags);
	}
}
