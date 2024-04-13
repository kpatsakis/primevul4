static void mct_u232_close(struct usb_serial_port *port)
{
	struct mct_u232_private *priv = usb_get_serial_port_data(port);

	usb_kill_urb(priv->read_urb);
	usb_kill_urb(port->interrupt_in_urb);

	usb_serial_generic_close(port);
} /* mct_u232_close */
