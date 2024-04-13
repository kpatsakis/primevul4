static void whiteheat_release(struct usb_serial *serial)
{
	struct usb_serial_port *command_port;

	/* free up our private data for our command port */
	command_port = serial->port[COMMAND_PORT];
	kfree(usb_get_serial_port_data(command_port));
}
