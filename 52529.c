static int mct_u232_set_modem_ctrl(struct usb_serial_port *port,
				   unsigned int control_state)
{
	int rc;
	unsigned char mcr;
	unsigned char *buf;

	buf = kmalloc(MCT_U232_MAX_SIZE, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	mcr = MCT_U232_MCR_NONE;
	if (control_state & TIOCM_DTR)
		mcr |= MCT_U232_MCR_DTR;
	if (control_state & TIOCM_RTS)
		mcr |= MCT_U232_MCR_RTS;

	buf[0] = mcr;
	rc = usb_control_msg(port->serial->dev, usb_sndctrlpipe(port->serial->dev, 0),
			MCT_U232_SET_MODEM_CTRL_REQUEST,
			MCT_U232_SET_REQUEST_TYPE,
			0, 0, buf, MCT_U232_SET_MODEM_CTRL_SIZE,
			WDR_TIMEOUT);
	kfree(buf);

	dev_dbg(&port->dev, "set_modem_ctrl: state=0x%x ==> mcr=0x%x\n", control_state, mcr);

	if (rc < 0) {
		dev_err(&port->dev, "Set MODEM CTRL 0x%x failed (error = %d)\n", mcr, rc);
		return rc;
	}
	return 0;
} /* mct_u232_set_modem_ctrl */
