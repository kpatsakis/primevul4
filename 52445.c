static void digi_rx_unthrottle(struct tty_struct *tty)
{
	int ret = 0;
	unsigned long flags;
	struct usb_serial_port *port = tty->driver_data;
	struct digi_port *priv = usb_get_serial_port_data(port);

	spin_lock_irqsave(&priv->dp_port_lock, flags);

	/* restart read chain */
	if (priv->dp_throttle_restart)
		ret = usb_submit_urb(port->read_urb, GFP_ATOMIC);

	/* turn throttle off */
	priv->dp_throttled = 0;
	priv->dp_throttle_restart = 0;

	spin_unlock_irqrestore(&priv->dp_port_lock, flags);

	if (ret)
		dev_err(&port->dev,
			"%s: usb_submit_urb failed, ret=%d, port=%d\n",
			__func__, ret, priv->dp_port_num);
}
