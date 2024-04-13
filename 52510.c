static void cypress_throttle(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct cypress_private *priv = usb_get_serial_port_data(port);

	spin_lock_irq(&priv->lock);
	priv->rx_flags = THROTTLED;
	spin_unlock_irq(&priv->lock);
}
