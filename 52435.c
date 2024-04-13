static int digi_chars_in_buffer(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct digi_port *priv = usb_get_serial_port_data(port);

	if (priv->dp_write_urb_in_use) {
		dev_dbg(&port->dev, "digi_chars_in_buffer: port=%d, chars=%d\n",
			priv->dp_port_num, port->bulk_out_size - 2);
		/* return(port->bulk_out_size - 2); */
		return 256;
	} else {
		dev_dbg(&port->dev, "digi_chars_in_buffer: port=%d, chars=%d\n",
			priv->dp_port_num, priv->dp_out_buf_len);
		return priv->dp_out_buf_len;
	}

}
