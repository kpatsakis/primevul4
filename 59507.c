static int  klsi_105_open(struct tty_struct *tty, struct usb_serial_port *port)
{
	struct klsi_105_private *priv = usb_get_serial_port_data(port);
	int retval = 0;
	int rc;
	int i;
	unsigned long line_state;
	struct klsi_105_port_settings *cfg;
	unsigned long flags;

	/* Do a defined restart:
	 * Set up sane default baud rate and send the 'READ_ON'
	 * vendor command.
	 * FIXME: set modem line control (how?)
	 * Then read the modem line control and store values in
	 * priv->line_state.
	 */
	cfg = kmalloc(sizeof(*cfg), GFP_KERNEL);
	if (!cfg)
		return -ENOMEM;

	cfg->pktlen   = 5;
	cfg->baudrate = kl5kusb105a_sio_b9600;
	cfg->databits = kl5kusb105a_dtb_8;
	cfg->unknown1 = 0;
	cfg->unknown2 = 1;
	klsi_105_chg_port_settings(port, cfg);

	/* set up termios structure */
	spin_lock_irqsave(&priv->lock, flags);
	priv->termios.c_iflag = tty->termios.c_iflag;
	priv->termios.c_oflag = tty->termios.c_oflag;
	priv->termios.c_cflag = tty->termios.c_cflag;
	priv->termios.c_lflag = tty->termios.c_lflag;
	for (i = 0; i < NCCS; i++)
		priv->termios.c_cc[i] = tty->termios.c_cc[i];
	priv->cfg.pktlen   = cfg->pktlen;
	priv->cfg.baudrate = cfg->baudrate;
	priv->cfg.databits = cfg->databits;
	priv->cfg.unknown1 = cfg->unknown1;
	priv->cfg.unknown2 = cfg->unknown2;
	spin_unlock_irqrestore(&priv->lock, flags);

	/* READ_ON and urb submission */
	rc = usb_serial_generic_open(tty, port);
	if (rc) {
		retval = rc;
		goto err_free_cfg;
	}

	rc = usb_control_msg(port->serial->dev,
			     usb_sndctrlpipe(port->serial->dev, 0),
			     KL5KUSB105A_SIO_CONFIGURE,
			     USB_TYPE_VENDOR|USB_DIR_OUT|USB_RECIP_INTERFACE,
			     KL5KUSB105A_SIO_CONFIGURE_READ_ON,
			     0, /* index */
			     NULL,
			     0,
			     KLSI_TIMEOUT);
	if (rc < 0) {
		dev_err(&port->dev, "Enabling read failed (error = %d)\n", rc);
		retval = rc;
		goto err_generic_close;
	} else
		dev_dbg(&port->dev, "%s - enabled reading\n", __func__);

	rc = klsi_105_get_line_state(port, &line_state);
	if (rc < 0) {
		retval = rc;
		goto err_disable_read;
	}

	spin_lock_irqsave(&priv->lock, flags);
	priv->line_state = line_state;
	spin_unlock_irqrestore(&priv->lock, flags);
	dev_dbg(&port->dev, "%s - read line state 0x%lx\n", __func__,
			line_state);

	return 0;

err_disable_read:
	usb_control_msg(port->serial->dev,
			     usb_sndctrlpipe(port->serial->dev, 0),
			     KL5KUSB105A_SIO_CONFIGURE,
			     USB_TYPE_VENDOR | USB_DIR_OUT,
			     KL5KUSB105A_SIO_CONFIGURE_READ_OFF,
			     0, /* index */
			     NULL, 0,
			     KLSI_TIMEOUT);
err_generic_close:
	usb_serial_generic_close(port);
err_free_cfg:
	kfree(cfg);

	return retval;
}
