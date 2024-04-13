static int digi_transmit_idle(struct usb_serial_port *port,
	unsigned long timeout)
{
	int ret;
	unsigned char buf[2];
	struct digi_port *priv = usb_get_serial_port_data(port);
	unsigned long flags = 0;

	spin_lock_irqsave(&priv->dp_port_lock, flags);
	priv->dp_transmit_idle = 0;
	spin_unlock_irqrestore(&priv->dp_port_lock, flags);

	buf[0] = DIGI_CMD_TRANSMIT_IDLE;
	buf[1] = 0;

	timeout += jiffies;

	ret = digi_write_inb_command(port, buf, 2, timeout - jiffies);
	if (ret != 0)
		return ret;

	spin_lock_irqsave(&priv->dp_port_lock, flags);

	while (time_before(jiffies, timeout) && !priv->dp_transmit_idle) {
		cond_wait_interruptible_timeout_irqrestore(
			&priv->dp_transmit_idle_wait, DIGI_RETRY_TIMEOUT,
			&priv->dp_port_lock, flags);
		if (signal_pending(current))
			return -EINTR;
		spin_lock_irqsave(&priv->dp_port_lock, flags);
	}
	priv->dp_transmit_idle = 0;
	spin_unlock_irqrestore(&priv->dp_port_lock, flags);
	return 0;

}
