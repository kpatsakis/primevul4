static void mct_u232_throttle(struct tty_struct *tty)
{
	struct usb_serial_port *port = tty->driver_data;
	struct mct_u232_private *priv = usb_get_serial_port_data(port);
	unsigned int control_state;

	spin_lock_irq(&priv->lock);
	priv->rx_flags |= THROTTLED;
	if (C_CRTSCTS(tty)) {
		priv->control_state &= ~TIOCM_RTS;
		control_state = priv->control_state;
		spin_unlock_irq(&priv->lock);
		mct_u232_set_modem_ctrl(port, control_state);
	} else {
		spin_unlock_irq(&priv->lock);
	}
}
