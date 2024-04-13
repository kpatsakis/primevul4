static void digi_set_termios(struct tty_struct *tty,
		struct usb_serial_port *port, struct ktermios *old_termios)
{
	struct digi_port *priv = usb_get_serial_port_data(port);
	struct device *dev = &port->dev;
	unsigned int iflag = tty->termios.c_iflag;
	unsigned int cflag = tty->termios.c_cflag;
	unsigned int old_iflag = old_termios->c_iflag;
	unsigned int old_cflag = old_termios->c_cflag;
	unsigned char buf[32];
	unsigned int modem_signals;
	int arg, ret;
	int i = 0;
	speed_t baud;

	dev_dbg(dev,
		"digi_set_termios: TOP: port=%d, iflag=0x%x, old_iflag=0x%x, cflag=0x%x, old_cflag=0x%x\n",
		priv->dp_port_num, iflag, old_iflag, cflag, old_cflag);

	/* set baud rate */
	baud = tty_get_baud_rate(tty);
	if (baud != tty_termios_baud_rate(old_termios)) {
		arg = -1;

		/* reassert DTR and (maybe) RTS on transition from B0 */
		if ((old_cflag & CBAUD) == B0) {
			/* don't set RTS if using hardware flow control */
			/* and throttling input */
			modem_signals = TIOCM_DTR;
			if (!C_CRTSCTS(tty) ||
			    !test_bit(TTY_THROTTLED, &tty->flags))
				modem_signals |= TIOCM_RTS;
			digi_set_modem_signals(port, modem_signals, 1);
		}
		switch (baud) {
		/* drop DTR and RTS on transition to B0 */
		case 0: digi_set_modem_signals(port, 0, 1); break;
		case 50: arg = DIGI_BAUD_50; break;
		case 75: arg = DIGI_BAUD_75; break;
		case 110: arg = DIGI_BAUD_110; break;
		case 150: arg = DIGI_BAUD_150; break;
		case 200: arg = DIGI_BAUD_200; break;
		case 300: arg = DIGI_BAUD_300; break;
		case 600: arg = DIGI_BAUD_600; break;
		case 1200: arg = DIGI_BAUD_1200; break;
		case 1800: arg = DIGI_BAUD_1800; break;
		case 2400: arg = DIGI_BAUD_2400; break;
		case 4800: arg = DIGI_BAUD_4800; break;
		case 9600: arg = DIGI_BAUD_9600; break;
		case 19200: arg = DIGI_BAUD_19200; break;
		case 38400: arg = DIGI_BAUD_38400; break;
		case 57600: arg = DIGI_BAUD_57600; break;
		case 115200: arg = DIGI_BAUD_115200; break;
		case 230400: arg = DIGI_BAUD_230400; break;
		case 460800: arg = DIGI_BAUD_460800; break;
		default:
			arg = DIGI_BAUD_9600;
			baud = 9600;
			break;
		}
		if (arg != -1) {
			buf[i++] = DIGI_CMD_SET_BAUD_RATE;
			buf[i++] = priv->dp_port_num;
			buf[i++] = arg;
			buf[i++] = 0;
		}
	}
	/* set parity */
	tty->termios.c_cflag &= ~CMSPAR;

	if ((cflag&(PARENB|PARODD)) != (old_cflag&(PARENB|PARODD))) {
		if (cflag&PARENB) {
			if (cflag&PARODD)
				arg = DIGI_PARITY_ODD;
			else
				arg = DIGI_PARITY_EVEN;
		} else {
			arg = DIGI_PARITY_NONE;
		}
		buf[i++] = DIGI_CMD_SET_PARITY;
		buf[i++] = priv->dp_port_num;
		buf[i++] = arg;
		buf[i++] = 0;
	}
	/* set word size */
	if ((cflag&CSIZE) != (old_cflag&CSIZE)) {
		arg = -1;
		switch (cflag&CSIZE) {
		case CS5: arg = DIGI_WORD_SIZE_5; break;
		case CS6: arg = DIGI_WORD_SIZE_6; break;
		case CS7: arg = DIGI_WORD_SIZE_7; break;
		case CS8: arg = DIGI_WORD_SIZE_8; break;
		default:
			dev_dbg(dev,
				"digi_set_termios: can't handle word size %d\n",
				(cflag&CSIZE));
			break;
		}

		if (arg != -1) {
			buf[i++] = DIGI_CMD_SET_WORD_SIZE;
			buf[i++] = priv->dp_port_num;
			buf[i++] = arg;
			buf[i++] = 0;
		}

	}

	/* set stop bits */
	if ((cflag&CSTOPB) != (old_cflag&CSTOPB)) {

		if ((cflag&CSTOPB))
			arg = DIGI_STOP_BITS_2;
		else
			arg = DIGI_STOP_BITS_1;

		buf[i++] = DIGI_CMD_SET_STOP_BITS;
		buf[i++] = priv->dp_port_num;
		buf[i++] = arg;
		buf[i++] = 0;

	}

	/* set input flow control */
	if ((iflag&IXOFF) != (old_iflag&IXOFF)
	    || (cflag&CRTSCTS) != (old_cflag&CRTSCTS)) {
		arg = 0;
		if (iflag&IXOFF)
			arg |= DIGI_INPUT_FLOW_CONTROL_XON_XOFF;
		else
			arg &= ~DIGI_INPUT_FLOW_CONTROL_XON_XOFF;

		if (cflag&CRTSCTS) {
			arg |= DIGI_INPUT_FLOW_CONTROL_RTS;

			/* On USB-4 it is necessary to assert RTS prior */
			/* to selecting RTS input flow control.  */
			buf[i++] = DIGI_CMD_SET_RTS_SIGNAL;
			buf[i++] = priv->dp_port_num;
			buf[i++] = DIGI_RTS_ACTIVE;
			buf[i++] = 0;

		} else {
			arg &= ~DIGI_INPUT_FLOW_CONTROL_RTS;
		}
		buf[i++] = DIGI_CMD_SET_INPUT_FLOW_CONTROL;
		buf[i++] = priv->dp_port_num;
		buf[i++] = arg;
		buf[i++] = 0;
	}

	/* set output flow control */
	if ((iflag & IXON) != (old_iflag & IXON)
	    || (cflag & CRTSCTS) != (old_cflag & CRTSCTS)) {
		arg = 0;
		if (iflag & IXON)
			arg |= DIGI_OUTPUT_FLOW_CONTROL_XON_XOFF;
		else
			arg &= ~DIGI_OUTPUT_FLOW_CONTROL_XON_XOFF;

		if (cflag & CRTSCTS) {
			arg |= DIGI_OUTPUT_FLOW_CONTROL_CTS;
		} else {
			arg &= ~DIGI_OUTPUT_FLOW_CONTROL_CTS;
		}

		buf[i++] = DIGI_CMD_SET_OUTPUT_FLOW_CONTROL;
		buf[i++] = priv->dp_port_num;
		buf[i++] = arg;
		buf[i++] = 0;
	}

	/* set receive enable/disable */
	if ((cflag & CREAD) != (old_cflag & CREAD)) {
		if (cflag & CREAD)
			arg = DIGI_ENABLE;
		else
			arg = DIGI_DISABLE;

		buf[i++] = DIGI_CMD_RECEIVE_ENABLE;
		buf[i++] = priv->dp_port_num;
		buf[i++] = arg;
		buf[i++] = 0;
	}
	ret = digi_write_oob_command(port, buf, i, 1);
	if (ret != 0)
		dev_dbg(dev, "digi_set_termios: write oob failed, ret=%d\n", ret);
	tty_encode_baud_rate(tty, baud, baud);
}
