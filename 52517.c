static void mct_u232_break_ctl(struct tty_struct *tty, int break_state)
{
	struct usb_serial_port *port = tty->driver_data;
	struct mct_u232_private *priv = usb_get_serial_port_data(port);
	unsigned char lcr;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	lcr = priv->last_lcr;

	if (break_state)
		lcr |= MCT_U232_SET_BREAK;
	spin_unlock_irqrestore(&priv->lock, flags);

	mct_u232_set_line_ctrl(port, lcr);
} /* mct_u232_break_ctl */
