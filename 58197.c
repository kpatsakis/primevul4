static int cxusb_d680_dmb_gpio_tuner(struct dvb_usb_device *d,
		u8 addr, int onoff)
{
	u8  o[2] = {addr, onoff};
	u8  i;
	int rc;

	rc = cxusb_ctrl_msg(d, CMD_GPIO_WRITE, o, 2, &i, 1);

	if (rc < 0)
		return rc;
	if (i == 0x01)
		return 0;
	else {
		deb_info("gpio_write failed.\n");
		return -EIO;
	}
}
