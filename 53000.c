static void iowarrior_write_callback(struct urb *urb)
{
	struct iowarrior *dev;
	int status = urb->status;

	dev = urb->context;
	/* sync/async unlink faults aren't errors */
	if (status &&
	    !(status == -ENOENT ||
	      status == -ECONNRESET || status == -ESHUTDOWN)) {
		dev_dbg(&dev->interface->dev,
			"nonzero write bulk status received: %d\n", status);
	}
	/* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			  urb->transfer_buffer, urb->transfer_dma);
	/* tell a waiting writer the interrupt-out-pipe is available again */
	atomic_dec(&dev->write_busy);
	wake_up_interruptible(&dev->write_wait);
}
