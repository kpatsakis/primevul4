static void iowarrior_callback(struct urb *urb)
{
	struct iowarrior *dev = urb->context;
	int intr_idx;
	int read_idx;
	int aux_idx;
	int offset;
	int status = urb->status;
	int retval;

	switch (status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		return;
	default:
		goto exit;
	}

	spin_lock(&dev->intr_idx_lock);
	intr_idx = atomic_read(&dev->intr_idx);
	/* aux_idx become previous intr_idx */
	aux_idx = (intr_idx == 0) ? (MAX_INTERRUPT_BUFFER - 1) : (intr_idx - 1);
	read_idx = atomic_read(&dev->read_idx);

	/* queue is not empty and it's interface 0 */
	if ((intr_idx != read_idx)
	    && (dev->interface->cur_altsetting->desc.bInterfaceNumber == 0)) {
		/* + 1 for serial number */
		offset = aux_idx * (dev->report_size + 1);
		if (!memcmp
		    (dev->read_queue + offset, urb->transfer_buffer,
		     dev->report_size)) {
			/* equal values on interface 0 will be ignored */
			spin_unlock(&dev->intr_idx_lock);
			goto exit;
		}
	}

	/* aux_idx become next intr_idx */
	aux_idx = (intr_idx == (MAX_INTERRUPT_BUFFER - 1)) ? 0 : (intr_idx + 1);
	if (read_idx == aux_idx) {
		/* queue full, dropping oldest input */
		read_idx = (++read_idx == MAX_INTERRUPT_BUFFER) ? 0 : read_idx;
		atomic_set(&dev->read_idx, read_idx);
		atomic_set(&dev->overflow_flag, 1);
	}

	/* +1 for serial number */
	offset = intr_idx * (dev->report_size + 1);
	memcpy(dev->read_queue + offset, urb->transfer_buffer,
	       dev->report_size);
	*(dev->read_queue + offset + (dev->report_size)) = dev->serial_number++;

	atomic_set(&dev->intr_idx, aux_idx);
	spin_unlock(&dev->intr_idx_lock);
	/* tell the blocking read about the new data */
	wake_up_interruptible(&dev->read_wait);

exit:
	retval = usb_submit_urb(urb, GFP_ATOMIC);
	if (retval)
		dev_err(&dev->interface->dev, "%s - usb_submit_urb failed with result %d\n",
			__func__, retval);

}
