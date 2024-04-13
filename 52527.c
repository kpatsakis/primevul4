static int mct_u232_set_baud_rate(struct tty_struct *tty,
	struct usb_serial *serial, struct usb_serial_port *port, speed_t value)
{
	unsigned int divisor;
	int rc;
	unsigned char *buf;
	unsigned char cts_enable_byte = 0;
	speed_t speed;

	buf = kmalloc(MCT_U232_MAX_SIZE, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	divisor = mct_u232_calculate_baud_rate(serial, value, &speed);
	put_unaligned_le32(cpu_to_le32(divisor), buf);
	rc = usb_control_msg(serial->dev, usb_sndctrlpipe(serial->dev, 0),
				MCT_U232_SET_BAUD_RATE_REQUEST,
				MCT_U232_SET_REQUEST_TYPE,
				0, 0, buf, MCT_U232_SET_BAUD_RATE_SIZE,
				WDR_TIMEOUT);
	if (rc < 0)	/*FIXME: What value speed results */
		dev_err(&port->dev, "Set BAUD RATE %d failed (error = %d)\n",
			value, rc);
	else
		tty_encode_baud_rate(tty, speed, speed);
	dev_dbg(&port->dev, "set_baud_rate: value: 0x%x, divisor: 0x%x\n", value, divisor);

	/* Mimic the MCT-supplied Windows driver (version 1.21P.0104), which
	   always sends two extra USB 'device request' messages after the
	   'baud rate change' message.  The actual functionality of the
	   request codes in these messages is not fully understood but these
	   particular codes are never seen in any operation besides a baud
	   rate change.  Both of these messages send a single byte of data.
	   In the first message, the value of this byte is always zero.

	   The second message has been determined experimentally to control
	   whether data will be transmitted to a device which is not asserting
	   the 'CTS' signal.  If the second message's data byte is zero, data
	   will be transmitted even if 'CTS' is not asserted (i.e. no hardware
	   flow control).  if the second message's data byte is nonzero (a
	   value of 1 is used by this driver), data will not be transmitted to
	   a device which is not asserting 'CTS'.
	*/

	buf[0] = 0;
	rc = usb_control_msg(serial->dev, usb_sndctrlpipe(serial->dev, 0),
				MCT_U232_SET_UNKNOWN1_REQUEST,
				MCT_U232_SET_REQUEST_TYPE,
				0, 0, buf, MCT_U232_SET_UNKNOWN1_SIZE,
				WDR_TIMEOUT);
	if (rc < 0)
		dev_err(&port->dev, "Sending USB device request code %d "
			"failed (error = %d)\n", MCT_U232_SET_UNKNOWN1_REQUEST,
			rc);

	if (port && C_CRTSCTS(tty))
	   cts_enable_byte = 1;

	dev_dbg(&port->dev, "set_baud_rate: send second control message, data = %02X\n",
		cts_enable_byte);
	buf[0] = cts_enable_byte;
	rc = usb_control_msg(serial->dev, usb_sndctrlpipe(serial->dev, 0),
			MCT_U232_SET_CTS_REQUEST,
			MCT_U232_SET_REQUEST_TYPE,
			0, 0, buf, MCT_U232_SET_CTS_SIZE,
			WDR_TIMEOUT);
	if (rc < 0)
		dev_err(&port->dev, "Sending USB device request code %d "
			"failed (error = %d)\n", MCT_U232_SET_CTS_REQUEST, rc);

	kfree(buf);
	return rc;
} /* mct_u232_set_baud_rate */
