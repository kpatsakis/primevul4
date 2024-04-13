static int cxusb_rc_query(struct dvb_usb_device *d)
{
	u8 ircode[4];

	cxusb_ctrl_msg(d, CMD_GET_IR_CODE, NULL, 0, ircode, 4);

	if (ircode[2] || ircode[3])
		rc_keydown(d->rc_dev, RC_TYPE_UNKNOWN,
			   RC_SCANCODE_RC5(ircode[2], ircode[3]), 0);
	return 0;
}
