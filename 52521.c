static int mct_u232_get_modem_stat(struct usb_serial_port *port,
				   unsigned char *msr)
{
	int rc;
	unsigned char *buf;

	buf = kmalloc(MCT_U232_MAX_SIZE, GFP_KERNEL);
	if (buf == NULL) {
		*msr = 0;
		return -ENOMEM;
	}
	rc = usb_control_msg(port->serial->dev, usb_rcvctrlpipe(port->serial->dev, 0),
			MCT_U232_GET_MODEM_STAT_REQUEST,
			MCT_U232_GET_REQUEST_TYPE,
			0, 0, buf, MCT_U232_GET_MODEM_STAT_SIZE,
			WDR_TIMEOUT);
	if (rc < 0) {
		dev_err(&port->dev, "Get MODEM STATus failed (error = %d)\n", rc);
		*msr = 0;
	} else {
		*msr = buf[0];
	}
	dev_dbg(&port->dev, "get_modem_stat: 0x%x\n", *msr);
	kfree(buf);
	return rc;
} /* mct_u232_get_modem_stat */
