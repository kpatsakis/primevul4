static int cxusb_bluebird2_rc_query(struct dvb_usb_device *d)
{
	u8 ircode[4];
	struct i2c_msg msg = { .addr = 0x6b, .flags = I2C_M_RD,
			       .buf = ircode, .len = 4 };

	if (cxusb_i2c_xfer(&d->i2c_adap, &msg, 1) != 1)
		return 0;

	if (ircode[1] || ircode[2])
		rc_keydown(d->rc_dev, RC_TYPE_UNKNOWN,
			   RC_SCANCODE_RC5(ircode[1], ircode[2]), 0);
	return 0;
}
