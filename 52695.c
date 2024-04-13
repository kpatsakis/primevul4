static int visor_open(struct tty_struct *tty, struct usb_serial_port *port)
{
	int result = 0;

	if (!port->read_urb) {
		/* this is needed for some brain dead Sony devices */
		dev_err(&port->dev, "Device lied about number of ports, please use a lower one.\n");
		return -ENODEV;
	}

	/* Start reading from the device */
	result = usb_serial_generic_open(tty, port);
	if (result)
		goto exit;

	if (port->interrupt_in_urb) {
		dev_dbg(&port->dev, "adding interrupt input for treo\n");
		result = usb_submit_urb(port->interrupt_in_urb, GFP_KERNEL);
		if (result)
			dev_err(&port->dev,
			    "%s - failed submitting interrupt urb, error %d\n",
							__func__, result);
	}
exit:
	return result;
}
