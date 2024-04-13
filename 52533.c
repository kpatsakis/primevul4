static int mct_u232_tiocmset(struct tty_struct *tty,
			      unsigned int set, unsigned int clear)
{
	struct usb_serial_port *port = tty->driver_data;
	struct mct_u232_private *priv = usb_get_serial_port_data(port);
	unsigned int control_state;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	control_state = priv->control_state;

	if (set & TIOCM_RTS)
		control_state |= TIOCM_RTS;
	if (set & TIOCM_DTR)
		control_state |= TIOCM_DTR;
	if (clear & TIOCM_RTS)
		control_state &= ~TIOCM_RTS;
	if (clear & TIOCM_DTR)
		control_state &= ~TIOCM_DTR;

	priv->control_state = control_state;
	spin_unlock_irqrestore(&priv->lock, flags);
	return mct_u232_set_modem_ctrl(port, control_state);
}
