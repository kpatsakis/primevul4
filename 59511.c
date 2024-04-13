static void klsi_105_set_termios(struct tty_struct *tty,
				 struct usb_serial_port *port,
				 struct ktermios *old_termios)
{
	struct klsi_105_private *priv = usb_get_serial_port_data(port);
	struct device *dev = &port->dev;
	unsigned int iflag = tty->termios.c_iflag;
	unsigned int old_iflag = old_termios->c_iflag;
	unsigned int cflag = tty->termios.c_cflag;
	unsigned int old_cflag = old_termios->c_cflag;
	struct klsi_105_port_settings *cfg;
	unsigned long flags;
	speed_t baud;

	cfg = kmalloc(sizeof(*cfg), GFP_KERNEL);
	if (!cfg)
		return;

	/* lock while we are modifying the settings */
	spin_lock_irqsave(&priv->lock, flags);

	/*
	 * Update baud rate
	 */
	baud = tty_get_baud_rate(tty);

	if ((cflag & CBAUD) != (old_cflag & CBAUD)) {
		/* reassert DTR and (maybe) RTS on transition from B0 */
		if ((old_cflag & CBAUD) == B0) {
			dev_dbg(dev, "%s: baud was B0\n", __func__);
#if 0
			priv->control_state |= TIOCM_DTR;
			/* don't set RTS if using hardware flow control */
			if (!(old_cflag & CRTSCTS))
				priv->control_state |= TIOCM_RTS;
			mct_u232_set_modem_ctrl(serial, priv->control_state);
#endif
		}
	}
	switch (baud) {
	case 0: /* handled below */
		break;
	case 1200:
		priv->cfg.baudrate = kl5kusb105a_sio_b1200;
		break;
	case 2400:
		priv->cfg.baudrate = kl5kusb105a_sio_b2400;
		break;
	case 4800:
		priv->cfg.baudrate = kl5kusb105a_sio_b4800;
		break;
	case 9600:
		priv->cfg.baudrate = kl5kusb105a_sio_b9600;
		break;
	case 19200:
		priv->cfg.baudrate = kl5kusb105a_sio_b19200;
		break;
	case 38400:
		priv->cfg.baudrate = kl5kusb105a_sio_b38400;
		break;
	case 57600:
		priv->cfg.baudrate = kl5kusb105a_sio_b57600;
		break;
	case 115200:
		priv->cfg.baudrate = kl5kusb105a_sio_b115200;
		break;
	default:
		dev_dbg(dev, "unsupported baudrate, using 9600\n");
		priv->cfg.baudrate = kl5kusb105a_sio_b9600;
		baud = 9600;
		break;
	}
	if ((cflag & CBAUD) == B0) {
		dev_dbg(dev, "%s: baud is B0\n", __func__);
		/* Drop RTS and DTR */
		/* maybe this should be simulated by sending read
		 * disable and read enable messages?
		 */
#if 0
		priv->control_state &= ~(TIOCM_DTR | TIOCM_RTS);
		mct_u232_set_modem_ctrl(serial, priv->control_state);
#endif
	}
	tty_encode_baud_rate(tty, baud, baud);

	if ((cflag & CSIZE) != (old_cflag & CSIZE)) {
		/* set the number of data bits */
		switch (cflag & CSIZE) {
		case CS5:
			dev_dbg(dev, "%s - 5 bits/byte not supported\n", __func__);
			spin_unlock_irqrestore(&priv->lock, flags);
			goto err;
		case CS6:
			dev_dbg(dev, "%s - 6 bits/byte not supported\n", __func__);
			spin_unlock_irqrestore(&priv->lock, flags);
			goto err;
		case CS7:
			priv->cfg.databits = kl5kusb105a_dtb_7;
			break;
		case CS8:
			priv->cfg.databits = kl5kusb105a_dtb_8;
			break;
		default:
			dev_err(dev, "CSIZE was not CS5-CS8, using default of 8\n");
			priv->cfg.databits = kl5kusb105a_dtb_8;
			break;
		}
	}

	/*
	 * Update line control register (LCR)
	 */
	if ((cflag & (PARENB|PARODD)) != (old_cflag & (PARENB|PARODD))
	    || (cflag & CSTOPB) != (old_cflag & CSTOPB)) {
		/* Not currently supported */
		tty->termios.c_cflag &= ~(PARENB|PARODD|CSTOPB);
#if 0
		priv->last_lcr = 0;

		/* set the parity */
		if (cflag & PARENB)
			priv->last_lcr |= (cflag & PARODD) ?
				MCT_U232_PARITY_ODD : MCT_U232_PARITY_EVEN;
		else
			priv->last_lcr |= MCT_U232_PARITY_NONE;

		/* set the number of stop bits */
		priv->last_lcr |= (cflag & CSTOPB) ?
			MCT_U232_STOP_BITS_2 : MCT_U232_STOP_BITS_1;

		mct_u232_set_line_ctrl(serial, priv->last_lcr);
#endif
	}
	/*
	 * Set flow control: well, I do not really now how to handle DTR/RTS.
	 * Just do what we have seen with SniffUSB on Win98.
	 */
	if ((iflag & IXOFF) != (old_iflag & IXOFF)
	    || (iflag & IXON) != (old_iflag & IXON)
	    ||  (cflag & CRTSCTS) != (old_cflag & CRTSCTS)) {
		/* Not currently supported */
		tty->termios.c_cflag &= ~CRTSCTS;
		/* Drop DTR/RTS if no flow control otherwise assert */
#if 0
		if ((iflag & IXOFF) || (iflag & IXON) || (cflag & CRTSCTS))
			priv->control_state |= TIOCM_DTR | TIOCM_RTS;
		else
			priv->control_state &= ~(TIOCM_DTR | TIOCM_RTS);
		mct_u232_set_modem_ctrl(serial, priv->control_state);
#endif
	}
	memcpy(cfg, &priv->cfg, sizeof(*cfg));
	spin_unlock_irqrestore(&priv->lock, flags);

	/* now commit changes to device */
	klsi_105_chg_port_settings(port, cfg);
err:
	kfree(cfg);
}
