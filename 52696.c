static int visor_probe(struct usb_serial *serial,
					const struct usb_device_id *id)
{
	int retval = 0;
	int (*startup)(struct usb_serial *serial,
					const struct usb_device_id *id);

	/*
	 * some Samsung Android phones in modem mode have the same ID
	 * as SPH-I500, but they are ACM devices, so dont bind to them
	 */
	if (id->idVendor == SAMSUNG_VENDOR_ID &&
		id->idProduct == SAMSUNG_SPH_I500_ID &&
		serial->dev->descriptor.bDeviceClass == USB_CLASS_COMM &&
		serial->dev->descriptor.bDeviceSubClass ==
			USB_CDC_SUBCLASS_ACM)
		return -ENODEV;

	if (serial->dev->actconfig->desc.bConfigurationValue != 1) {
		dev_err(&serial->dev->dev, "active config #%d != 1 ??\n",
			serial->dev->actconfig->desc.bConfigurationValue);
		return -ENODEV;
	}

	if (id->driver_info) {
		startup = (void *)id->driver_info;
		retval = startup(serial, id);
	}

	return retval;
}
