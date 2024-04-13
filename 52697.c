static void visor_read_int_callback(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	int status = urb->status;
	int result;

	switch (status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dev_dbg(&port->dev, "%s - urb shutting down with status: %d\n",
			__func__, status);
		return;
	default:
		dev_dbg(&port->dev, "%s - nonzero urb status received: %d\n",
			__func__, status);
		goto exit;
	}

	/*
	 * This information is still unknown what it can be used for.
	 * If anyone has an idea, please let the author know...
	 *
	 * Rumor has it this endpoint is used to notify when data
	 * is ready to be read from the bulk ones.
	 */
	usb_serial_debug_data(&port->dev, __func__, urb->actual_length,
			      urb->transfer_buffer);

exit:
	result = usb_submit_urb(urb, GFP_ATOMIC);
	if (result)
		dev_err(&urb->dev->dev,
				"%s - Error %d submitting interrupt urb\n",
							__func__, result);
}
