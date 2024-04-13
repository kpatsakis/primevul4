static int analyze_baud_rate(struct usb_serial_port *port, speed_t new_rate)
{
	struct cypress_private *priv;
	priv = usb_get_serial_port_data(port);

	if (unstable_bauds)
		return new_rate;

	/* FRWD Dongle uses 115200 bps */
	if (is_frwd(port->serial->dev))
		return new_rate;

	/*
	 * The general purpose firmware for the Cypress M8 allows for
	 * a maximum speed of 57600bps (I have no idea whether DeLorme
	 * chose to use the general purpose firmware or not), if you
	 * need to modify this speed setting for your own project
	 * please add your own chiptype and modify the code likewise.
	 * The Cypress HID->COM device will work successfully up to
	 * 115200bps (but the actual throughput is around 3kBps).
	 */
	if (port->serial->dev->speed == USB_SPEED_LOW) {
		/*
		 * Mike Isely <isely@pobox.com> 2-Feb-2008: The
		 * Cypress app note that describes this mechanism
		 * states the the low-speed part can't handle more
		 * than 800 bytes/sec, in which case 4800 baud is the
		 * safest speed for a part like that.
		 */
		if (new_rate > 4800) {
			dev_dbg(&port->dev,
				"%s - failed setting baud rate, device incapable speed %d\n",
				__func__, new_rate);
			return -1;
		}
	}
	switch (priv->chiptype) {
	case CT_EARTHMATE:
		if (new_rate <= 600) {
			/* 300 and 600 baud rates are supported under
			 * the generic firmware, but are not used with
			 * NMEA and SiRF protocols */
			dev_dbg(&port->dev,
				"%s - failed setting baud rate, unsupported speed of %d on Earthmate GPS\n",
				__func__, new_rate);
			return -1;
		}
		break;
	default:
		break;
	}
	return new_rate;
}
