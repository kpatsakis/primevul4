static int mct_u232_tiocmget(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct mct_u232_private *priv = usb_get_serial_port_data(port);
	unsigned int control_state;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	control_state = priv->control_state;
	spin_unlock_irqrestore(&priv->lock, flags);

	return control_state;
}
