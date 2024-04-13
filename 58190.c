static void cxusb_bluebird_gpio_pulse(struct dvb_usb_device *d, u8 pin, int low)
{
	cxusb_bluebird_gpio_rw(d, pin, low ? 0 : pin);
	msleep(5);
	cxusb_bluebird_gpio_rw(d, pin, low ? pin : 0);
}
