static int omninet_attach(struct usb_serial *serial)
{
	/* The second bulk-out endpoint is used for writing. */
	if (serial->num_bulk_out < 2) {
		dev_err(&serial->interface->dev, "missing endpoints\n");
		return -ENODEV;
	}

	return 0;
}
