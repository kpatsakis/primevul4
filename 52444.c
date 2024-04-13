static void digi_rx_throttle(struct tty_struct *tty)
{
	unsigned long flags;
	struct usb_serial_port *port = tty->driver_data;
	struct digi_port *priv = usb_get_serial_port_data(port);

	/* stop receiving characters by not resubmitting the read urb */
	spin_lock_irqsave(&priv->dp_port_lock, flags);
	priv->dp_throttled = 1;
	priv->dp_throttle_restart = 0;
	spin_unlock_irqrestore(&priv->dp_port_lock, flags);
}
