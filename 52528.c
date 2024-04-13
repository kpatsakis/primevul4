static int mct_u232_set_line_ctrl(struct usb_serial_port *port,
				  unsigned char lcr)
{
	int rc;
	unsigned char *buf;

	buf = kmalloc(MCT_U232_MAX_SIZE, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	buf[0] = lcr;
	rc = usb_control_msg(port->serial->dev, usb_sndctrlpipe(port->serial->dev, 0),
			MCT_U232_SET_LINE_CTRL_REQUEST,
			MCT_U232_SET_REQUEST_TYPE,
			0, 0, buf, MCT_U232_SET_LINE_CTRL_SIZE,
			WDR_TIMEOUT);
	if (rc < 0)
		dev_err(&port->dev, "Set LINE CTRL 0x%x failed (error = %d)\n", lcr, rc);
	dev_dbg(&port->dev, "set_line_ctrl: 0x%x\n", lcr);
	kfree(buf);
	return rc;
} /* mct_u232_set_line_ctrl */
