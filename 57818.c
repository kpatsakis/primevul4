static void omninet_process_read_urb(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	const struct omninet_header *hdr = urb->transfer_buffer;
	const unsigned char *data;
	size_t data_len;

	if (urb->actual_length <= OMNINET_HEADERLEN || !hdr->oh_len)
		return;

	data = (char *)urb->transfer_buffer + OMNINET_HEADERLEN;
	data_len = min_t(size_t, urb->actual_length - OMNINET_HEADERLEN,
								hdr->oh_len);
	tty_insert_flip_string(&port->port, data, data_len);
	tty_flip_buffer_push(&port->port);
}
