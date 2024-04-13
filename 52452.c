static void digi_wakeup_write_lock(struct work_struct *work)
{
	struct digi_port *priv =
			container_of(work, struct digi_port, dp_wakeup_work);
	struct usb_serial_port *port = priv->dp_port;
	unsigned long flags;

	spin_lock_irqsave(&priv->dp_port_lock, flags);
	tty_port_tty_wakeup(&port->port);
	spin_unlock_irqrestore(&priv->dp_port_lock, flags);
}
