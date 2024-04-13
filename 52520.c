static void mct_u232_dtr_rts(struct usb_serial_port *port, int on)
{
	unsigned int control_state;
	struct mct_u232_private *priv = usb_get_serial_port_data(port);

	spin_lock_irq(&priv->lock);
	if (on)
		priv->control_state |= TIOCM_DTR | TIOCM_RTS;
	else
		priv->control_state &= ~(TIOCM_DTR | TIOCM_RTS);
	control_state = priv->control_state;
	spin_unlock_irq(&priv->lock);

	mct_u232_set_modem_ctrl(port, control_state);
}
