static int cxusb_d680_dmb_power_ctrl(struct dvb_usb_device *d, int onoff)
{
	int ret;
	u8  b;
	ret = cxusb_power_ctrl(d, onoff);
	if (!onoff)
		return ret;

	msleep(128);
	cxusb_ctrl_msg(d, CMD_DIGITAL, NULL, 0, &b, 1);
	msleep(100);
	return ret;
}
