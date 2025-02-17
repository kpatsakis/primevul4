static void change_port_settings(struct tty_struct *tty,
		struct edgeport_port *edge_port, struct ktermios *old_termios)
{
	struct device *dev = &edge_port->port->dev;
	struct ump_uart_config *config;
	int baud;
	unsigned cflag;
	int status;
	int port_number = edge_port->port->number -
					edge_port->port->serial->minor;

	dev_dbg(dev, "%s - port %d\n", __func__, edge_port->port->number);

	config = kmalloc (sizeof (*config), GFP_KERNEL);
	if (!config) {
		tty->termios = *old_termios;
		dev_err(dev, "%s - out of memory\n", __func__);
		return;
	}

	cflag = tty->termios.c_cflag;

	config->wFlags = 0;

	/* These flags must be set */
	config->wFlags |= UMP_MASK_UART_FLAGS_RECEIVE_MS_INT;
	config->wFlags |= UMP_MASK_UART_FLAGS_AUTO_START_ON_ERR;
	config->bUartMode = (__u8)(edge_port->bUartMode);

	switch (cflag & CSIZE) {
	case CS5:
		    config->bDataBits = UMP_UART_CHAR5BITS;
		    dev_dbg(dev, "%s - data bits = 5\n", __func__);
		    break;
	case CS6:
		    config->bDataBits = UMP_UART_CHAR6BITS;
		    dev_dbg(dev, "%s - data bits = 6\n", __func__);
		    break;
	case CS7:
		    config->bDataBits = UMP_UART_CHAR7BITS;
		    dev_dbg(dev, "%s - data bits = 7\n", __func__);
		    break;
	default:
	case CS8:
		    config->bDataBits = UMP_UART_CHAR8BITS;
		    dev_dbg(dev, "%s - data bits = 8\n", __func__);
			    break;
	}

	if (cflag & PARENB) {
		if (cflag & PARODD) {
			config->wFlags |= UMP_MASK_UART_FLAGS_PARITY;
			config->bParity = UMP_UART_ODDPARITY;
			dev_dbg(dev, "%s - parity = odd\n", __func__);
		} else {
			config->wFlags |= UMP_MASK_UART_FLAGS_PARITY;
			config->bParity = UMP_UART_EVENPARITY;
			dev_dbg(dev, "%s - parity = even\n", __func__);
		}
	} else {
		config->bParity = UMP_UART_NOPARITY;
		dev_dbg(dev, "%s - parity = none\n", __func__);
	}

	if (cflag & CSTOPB) {
		config->bStopBits = UMP_UART_STOPBIT2;
		dev_dbg(dev, "%s - stop bits = 2\n", __func__);
	} else {
		config->bStopBits = UMP_UART_STOPBIT1;
		dev_dbg(dev, "%s - stop bits = 1\n", __func__);
	}

	/* figure out the flow control settings */
	if (cflag & CRTSCTS) {
		config->wFlags |= UMP_MASK_UART_FLAGS_OUT_X_CTS_FLOW;
		config->wFlags |= UMP_MASK_UART_FLAGS_RTS_FLOW;
		dev_dbg(dev, "%s - RTS/CTS is enabled\n", __func__);
	} else {
		dev_dbg(dev, "%s - RTS/CTS is disabled\n", __func__);
		tty->hw_stopped = 0;
		restart_read(edge_port);
	}

	/* if we are implementing XON/XOFF, set the start and stop
	   character in the device */
	config->cXon  = START_CHAR(tty);
	config->cXoff = STOP_CHAR(tty);

	/* if we are implementing INBOUND XON/XOFF */
	if (I_IXOFF(tty)) {
		config->wFlags |= UMP_MASK_UART_FLAGS_IN_X;
		dev_dbg(dev, "%s - INBOUND XON/XOFF is enabled, XON = %2x, XOFF = %2x\n",
			__func__, config->cXon, config->cXoff);
	} else
		dev_dbg(dev, "%s - INBOUND XON/XOFF is disabled\n", __func__);

	/* if we are implementing OUTBOUND XON/XOFF */
	if (I_IXON(tty)) {
		config->wFlags |= UMP_MASK_UART_FLAGS_OUT_X;
		dev_dbg(dev, "%s - OUTBOUND XON/XOFF is enabled, XON = %2x, XOFF = %2x\n",
			__func__, config->cXon, config->cXoff);
	} else
		dev_dbg(dev, "%s - OUTBOUND XON/XOFF is disabled\n", __func__);

	tty->termios.c_cflag &= ~CMSPAR;

	/* Round the baud rate */
	baud = tty_get_baud_rate(tty);
	if (!baud) {
		/* pick a default, any default... */
		baud = 9600;
	} else
		tty_encode_baud_rate(tty, baud, baud);

	edge_port->baud_rate = baud;
	config->wBaudRate = (__u16)((461550L + baud/2) / baud);

	/* FIXME: Recompute actual baud from divisor here */

	dev_dbg(dev, "%s - baud rate = %d, wBaudRate = %d\n", __func__, baud, config->wBaudRate);

	dev_dbg(dev, "wBaudRate:   %d\n", (int)(461550L / config->wBaudRate));
	dev_dbg(dev, "wFlags:    0x%x\n", config->wFlags);
	dev_dbg(dev, "bDataBits:   %d\n", config->bDataBits);
	dev_dbg(dev, "bParity:     %d\n", config->bParity);
	dev_dbg(dev, "bStopBits:   %d\n", config->bStopBits);
	dev_dbg(dev, "cXon:        %d\n", config->cXon);
	dev_dbg(dev, "cXoff:       %d\n", config->cXoff);
	dev_dbg(dev, "bUartMode:   %d\n", config->bUartMode);

	/* move the word values into big endian mode */
	cpu_to_be16s(&config->wFlags);
	cpu_to_be16s(&config->wBaudRate);

	status = send_cmd(edge_port->port->serial->dev, UMPC_SET_CONFIG,
				(__u8)(UMPM_UART1_PORT + port_number),
				0, (__u8 *)config, sizeof(*config));
	if (status)
		dev_dbg(dev, "%s - error %d when trying to write config to device\n",
			__func__, status);
	kfree(config);
}
