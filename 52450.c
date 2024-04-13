static int digi_tiocmset(struct tty_struct *tty,
					unsigned int set, unsigned int clear)
{
	struct usb_serial_port *port = tty->driver_data;
	struct digi_port *priv = usb_get_serial_port_data(port);
	unsigned int val;
	unsigned long flags;

	spin_lock_irqsave(&priv->dp_port_lock, flags);
	val = (priv->dp_modem_signals & ~clear) | set;
	spin_unlock_irqrestore(&priv->dp_port_lock, flags);
	return digi_set_modem_signals(port, val, 1);
}
