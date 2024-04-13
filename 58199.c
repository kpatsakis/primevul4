static int cxusb_d680_dmb_rc_query(struct dvb_usb_device *d)
{
	u8 ircode[2];

	if (cxusb_ctrl_msg(d, 0x10, NULL, 0, ircode, 2) < 0)
		return 0;

	if (ircode[0] || ircode[1])
		rc_keydown(d->rc_dev, RC_TYPE_UNKNOWN,
			   RC_SCANCODE_RC5(ircode[0], ircode[1]), 0);
	return 0;
}
