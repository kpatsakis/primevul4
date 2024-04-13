static void edge_bulk_out_callback(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	struct edgeport_port *edge_port = usb_get_serial_port_data(port);
	int status = urb->status;
	struct tty_struct *tty;

	edge_port->ep_write_urb_in_use = 0;

	switch (status) {
	case 0:
		/* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* this urb is terminated, clean up */
		dev_dbg(&urb->dev->dev, "%s - urb shutting down with status: %d\n",
		    __func__, status);
		return;
	default:
		dev_err_console(port, "%s - nonzero write bulk status "
			"received: %d\n", __func__, status);
	}

	/* send any buffered data */
	tty = tty_port_tty_get(&port->port);
	edge_send(port, tty);
	tty_kref_put(tty);
}
