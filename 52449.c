static int digi_tiocmget(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct digi_port *priv = usb_get_serial_port_data(port);
	unsigned int val;
	unsigned long flags;

	spin_lock_irqsave(&priv->dp_port_lock, flags);
	val = priv->dp_modem_signals;
	spin_unlock_irqrestore(&priv->dp_port_lock, flags);
	return val;
}
