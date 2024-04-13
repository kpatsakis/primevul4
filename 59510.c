static void klsi_105_process_read_urb(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	unsigned char *data = urb->transfer_buffer;
	unsigned len;

	/* empty urbs seem to happen, we ignore them */
	if (!urb->actual_length)
		return;

	if (urb->actual_length <= KLSI_HDR_LEN) {
		dev_dbg(&port->dev, "%s - malformed packet\n", __func__);
		return;
	}

	len = get_unaligned_le16(data);
	if (len > urb->actual_length - KLSI_HDR_LEN) {
		dev_dbg(&port->dev, "%s - packet length mismatch\n", __func__);
		len = urb->actual_length - KLSI_HDR_LEN;
	}

	tty_insert_flip_string(&port->port, data + KLSI_HDR_LEN, len);
	tty_flip_buffer_push(&port->port);
}
