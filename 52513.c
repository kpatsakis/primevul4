static void cypress_unthrottle(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct cypress_private *priv = usb_get_serial_port_data(port);
	int actually_throttled, result;

	spin_lock_irq(&priv->lock);
	actually_throttled = priv->rx_flags & ACTUALLY_THROTTLED;
	priv->rx_flags = 0;
	spin_unlock_irq(&priv->lock);

	if (!priv->comm_is_ok)
		return;

	if (actually_throttled) {
		result = usb_submit_urb(port->interrupt_in_urb, GFP_KERNEL);
		if (result) {
			dev_err(&port->dev, "%s - failed submitting read urb, "
					"error %d\n", __func__, result);
			cypress_set_dead(port);
		}
	}
}
