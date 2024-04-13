static void tower_interrupt_in_callback (struct urb *urb)
{
	struct lego_usb_tower *dev = urb->context;
	int status = urb->status;
	int retval;

	lego_usb_tower_debug_data(&dev->udev->dev, __func__,
				  urb->actual_length, urb->transfer_buffer);

	if (status) {
		if (status == -ENOENT ||
		    status == -ECONNRESET ||
		    status == -ESHUTDOWN) {
			goto exit;
		} else {
			dev_dbg(&dev->udev->dev,
				"%s: nonzero status received: %d\n", __func__,
				status);
			goto resubmit; /* maybe we can recover */
		}
	}

	if (urb->actual_length > 0) {
		spin_lock (&dev->read_buffer_lock);
		if (dev->read_buffer_length + urb->actual_length < read_buffer_size) {
			memcpy (dev->read_buffer + dev->read_buffer_length,
				dev->interrupt_in_buffer,
				urb->actual_length);
			dev->read_buffer_length += urb->actual_length;
			dev->read_last_arrival = jiffies;
			dev_dbg(&dev->udev->dev, "%s: received %d bytes\n",
				__func__, urb->actual_length);
		} else {
			pr_warn("read_buffer overflow, %d bytes dropped\n",
				urb->actual_length);
		}
		spin_unlock (&dev->read_buffer_lock);
	}

resubmit:
	/* resubmit if we're still running */
	if (dev->interrupt_in_running && dev->udev) {
		retval = usb_submit_urb (dev->interrupt_in_urb, GFP_ATOMIC);
		if (retval)
			dev_err(&dev->udev->dev,
				"%s: usb_submit_urb failed (%d)\n",
				__func__, retval);
	}

exit:
	dev->interrupt_in_done = 1;
	wake_up_interruptible (&dev->read_wait);
}
