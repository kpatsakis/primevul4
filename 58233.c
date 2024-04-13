static int dvico_bluebird_xc2028_callback(void *ptr, int component,
					  int command, int arg)
{
	struct dvb_usb_adapter *adap = ptr;
	struct dvb_usb_device *d = adap->dev;

	switch (command) {
	case XC2028_TUNER_RESET:
		deb_info("%s: XC2028_TUNER_RESET %d\n", __func__, arg);
		cxusb_bluebird_gpio_pulse(d, 0x01, 1);
		break;
	case XC2028_RESET_CLK:
		deb_info("%s: XC2028_RESET_CLK %d\n", __func__, arg);
		break;
	default:
		deb_info("%s: unknown command %d, arg %d\n", __func__,
			 command, arg);
		return -EINVAL;
	}

	return 0;
}
