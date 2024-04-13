static void tower_interrupt_out_callback (struct urb *urb)
{
	struct lego_usb_tower *dev = urb->context;
	int status = urb->status;

	lego_usb_tower_debug_data(&dev->udev->dev, __func__,
				  urb->actual_length, urb->transfer_buffer);

	/* sync/async unlink faults aren't errors */
	if (status && !(status == -ENOENT ||
			status == -ECONNRESET ||
			status == -ESHUTDOWN)) {
		dev_dbg(&dev->udev->dev,
			"%s: nonzero write bulk status received: %d\n", __func__,
			status);
	}

	dev->interrupt_out_busy = 0;
	wake_up_interruptible(&dev->write_wait);
}
