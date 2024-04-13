static int mct_u232_calculate_baud_rate(struct usb_serial *serial,
					speed_t value, speed_t *result)
{
	*result = value;

	if (le16_to_cpu(serial->dev->descriptor.idProduct) == MCT_U232_SITECOM_PID
		|| le16_to_cpu(serial->dev->descriptor.idProduct) == MCT_U232_BELKIN_F5U109_PID) {
		switch (value) {
		case 300:
			return 0x01;
		case 600:
			return 0x02; /* this one not tested */
		case 1200:
			return 0x03;
		case 2400:
			return 0x04;
		case 4800:
			return 0x06;
		case 9600:
			return 0x08;
		case 19200:
			return 0x09;
		case 38400:
			return 0x0a;
		case 57600:
			return 0x0b;
		case 115200:
			return 0x0c;
		default:
			*result = 9600;
			return 0x08;
		}
	} else {
		/* FIXME: Can we use any divider - should we do
		   divider = 115200/value;
		   real baud = 115200/divider */
		switch (value) {
		case 300: break;
		case 600: break;
		case 1200: break;
		case 2400: break;
		case 4800: break;
		case 9600: break;
		case 19200: break;
		case 38400: break;
		case 57600: break;
		case 115200: break;
		default:
			value = 9600;
			*result = 9600;
		}
		return 115200/value;
	}
}
