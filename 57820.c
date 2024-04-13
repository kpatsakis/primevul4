static void omninet_write_bulk_callback(struct urb *urb)
{
/*	struct omninet_header	*header = (struct omninet_header  *)
						urb->transfer_buffer; */
	struct usb_serial_port 	*port   =  urb->context;
	int status = urb->status;

	set_bit(0, &port->write_urbs_free);
	if (status) {
		dev_dbg(&port->dev, "%s - nonzero write bulk status received: %d\n",
			__func__, status);
		return;
	}

	usb_serial_port_softint(port);
}
