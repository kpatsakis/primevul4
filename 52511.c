static int cypress_tiocmget(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct cypress_private *priv = usb_get_serial_port_data(port);
	__u8 status, control;
	unsigned int result = 0;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	control = priv->line_control;
	status = priv->current_status;
	spin_unlock_irqrestore(&priv->lock, flags);

	result = ((control & CONTROL_DTR)        ? TIOCM_DTR : 0)
		| ((control & CONTROL_RTS)       ? TIOCM_RTS : 0)
		| ((status & UART_CTS)        ? TIOCM_CTS : 0)
		| ((status & UART_DSR)        ? TIOCM_DSR : 0)
		| ((status & UART_RI)         ? TIOCM_RI  : 0)
		| ((status & UART_CD)         ? TIOCM_CD  : 0);

	dev_dbg(&port->dev, "%s - result = %x\n", __func__, result);

	return result;
}
