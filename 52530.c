static void mct_u232_set_termios(struct tty_struct *tty,
				 struct usb_serial_port *port,
				 struct ktermios *old_termios)
{
	struct usb_serial *serial = port->serial;
	struct mct_u232_private *priv = usb_get_serial_port_data(port);
	struct ktermios *termios = &tty->termios;
	unsigned int cflag = termios->c_cflag;
	unsigned int old_cflag = old_termios->c_cflag;
	unsigned long flags;
	unsigned int control_state;
	unsigned char last_lcr;

	/* get a local copy of the current port settings */
	spin_lock_irqsave(&priv->lock, flags);
	control_state = priv->control_state;
	spin_unlock_irqrestore(&priv->lock, flags);
	last_lcr = 0;

	/*
	 * Update baud rate.
	 * Do not attempt to cache old rates and skip settings,
	 * disconnects screw such tricks up completely.
	 * Premature optimization is the root of all evil.
	 */

	/* reassert DTR and RTS on transition from B0 */
	if ((old_cflag & CBAUD) == B0) {
		dev_dbg(&port->dev, "%s: baud was B0\n", __func__);
		control_state |= TIOCM_DTR | TIOCM_RTS;
		mct_u232_set_modem_ctrl(port, control_state);
	}

	mct_u232_set_baud_rate(tty, serial, port, tty_get_baud_rate(tty));

	if ((cflag & CBAUD) == B0) {
		dev_dbg(&port->dev, "%s: baud is B0\n", __func__);
		/* Drop RTS and DTR */
		control_state &= ~(TIOCM_DTR | TIOCM_RTS);
		mct_u232_set_modem_ctrl(port, control_state);
	}

	/*
	 * Update line control register (LCR)
	 */

	/* set the parity */
	if (cflag & PARENB)
		last_lcr |= (cflag & PARODD) ?
			MCT_U232_PARITY_ODD : MCT_U232_PARITY_EVEN;
	else
		last_lcr |= MCT_U232_PARITY_NONE;

	/* set the number of data bits */
	switch (cflag & CSIZE) {
	case CS5:
		last_lcr |= MCT_U232_DATA_BITS_5; break;
	case CS6:
		last_lcr |= MCT_U232_DATA_BITS_6; break;
	case CS7:
		last_lcr |= MCT_U232_DATA_BITS_7; break;
	case CS8:
		last_lcr |= MCT_U232_DATA_BITS_8; break;
	default:
		dev_err(&port->dev,
			"CSIZE was not CS5-CS8, using default of 8\n");
		last_lcr |= MCT_U232_DATA_BITS_8;
		break;
	}

	termios->c_cflag &= ~CMSPAR;

	/* set the number of stop bits */
	last_lcr |= (cflag & CSTOPB) ?
		MCT_U232_STOP_BITS_2 : MCT_U232_STOP_BITS_1;

	mct_u232_set_line_ctrl(port, last_lcr);

	/* save off the modified port settings */
	spin_lock_irqsave(&priv->lock, flags);
	priv->control_state = control_state;
	priv->last_lcr = last_lcr;
	spin_unlock_irqrestore(&priv->lock, flags);
} /* mct_u232_set_termios */
