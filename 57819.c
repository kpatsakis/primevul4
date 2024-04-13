static int omninet_write(struct tty_struct *tty, struct usb_serial_port *port,
					const unsigned char *buf, int count)
{
	struct usb_serial *serial = port->serial;
	struct usb_serial_port *wport = serial->port[1];

	struct omninet_data *od = usb_get_serial_port_data(port);
	struct omninet_header *header = (struct omninet_header *)
					wport->write_urb->transfer_buffer;

	int			result;

	if (count == 0) {
		dev_dbg(&port->dev, "%s - write request of 0 bytes\n", __func__);
		return 0;
	}

	if (!test_and_clear_bit(0, &port->write_urbs_free)) {
		dev_dbg(&port->dev, "%s - already writing\n", __func__);
		return 0;
	}

	count = (count > OMNINET_PAYLOADSIZE) ? OMNINET_PAYLOADSIZE : count;

	memcpy(wport->write_urb->transfer_buffer + OMNINET_HEADERLEN,
								buf, count);

	usb_serial_debug_data(&port->dev, __func__, count,
			      wport->write_urb->transfer_buffer);

	header->oh_seq 	= od->od_outseq++;
	header->oh_len 	= count;
	header->oh_xxx  = 0x03;
	header->oh_pad 	= 0x00;

	/* send the data out the bulk port, always 64 bytes */
	wport->write_urb->transfer_buffer_length = OMNINET_BULKOUTSIZE;

	result = usb_submit_urb(wport->write_urb, GFP_ATOMIC);
	if (result) {
		set_bit(0, &wport->write_urbs_free);
		dev_err_console(port,
			"%s - failed submitting write urb, error %d\n",
			__func__, result);
	} else
		result = count;

	return result;
}
