static int  mct_u232_open(struct tty_struct *tty, struct usb_serial_port *port)
{
	struct usb_serial *serial = port->serial;
	struct mct_u232_private *priv = usb_get_serial_port_data(port);
	int retval = 0;
	unsigned int control_state;
	unsigned long flags;
	unsigned char last_lcr;
	unsigned char last_msr;

	/* Compensate for a hardware bug: although the Sitecom U232-P25
	 * device reports a maximum output packet size of 32 bytes,
	 * it seems to be able to accept only 16 bytes (and that's what
	 * SniffUSB says too...)
	 */
	if (le16_to_cpu(serial->dev->descriptor.idProduct)
						== MCT_U232_SITECOM_PID)
		port->bulk_out_size = 16;

	/* Do a defined restart: the normal serial device seems to
	 * always turn on DTR and RTS here, so do the same. I'm not
	 * sure if this is really necessary. But it should not harm
	 * either.
	 */
	spin_lock_irqsave(&priv->lock, flags);
	if (tty && C_BAUD(tty))
		priv->control_state = TIOCM_DTR | TIOCM_RTS;
	else
		priv->control_state = 0;

	priv->last_lcr = (MCT_U232_DATA_BITS_8 |
			  MCT_U232_PARITY_NONE |
			  MCT_U232_STOP_BITS_1);
	control_state = priv->control_state;
	last_lcr = priv->last_lcr;
	spin_unlock_irqrestore(&priv->lock, flags);
	mct_u232_set_modem_ctrl(port, control_state);
	mct_u232_set_line_ctrl(port, last_lcr);

	/* Read modem status and update control state */
	mct_u232_get_modem_stat(port, &last_msr);
	spin_lock_irqsave(&priv->lock, flags);
	priv->last_msr = last_msr;
	mct_u232_msr_to_state(port, &priv->control_state, priv->last_msr);
	spin_unlock_irqrestore(&priv->lock, flags);

	retval = usb_submit_urb(priv->read_urb, GFP_KERNEL);
	if (retval) {
		dev_err(&port->dev,
			"usb_submit_urb(read) failed pipe 0x%x err %d\n",
			port->read_urb->pipe, retval);
		goto error;
	}

	retval = usb_submit_urb(port->interrupt_in_urb, GFP_KERNEL);
	if (retval) {
		usb_kill_urb(priv->read_urb);
		dev_err(&port->dev,
			"usb_submit_urb(read int) failed pipe 0x%x err %d",
			port->interrupt_in_urb->pipe, retval);
		goto error;
	}
	return 0;

error:
	return retval;
} /* mct_u232_open */
