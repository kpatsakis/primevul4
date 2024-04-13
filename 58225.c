static void cxusb_nano2_led(struct dvb_usb_device *d, int onoff)
{
	cxusb_bluebird_gpio_rw(d, 0x40, onoff ? 0 : 0x40);
}
