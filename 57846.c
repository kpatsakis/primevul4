static void edge_tty_recv(struct usb_serial_port *port, unsigned char *data,
		int length)
{
	int queued;

	queued = tty_insert_flip_string(&port->port, data, length);
	if (queued < length)
		dev_err(&port->dev, "%s - dropping data, %d bytes lost\n",
			__func__, length - queued);
	tty_flip_buffer_push(&port->port);
}
