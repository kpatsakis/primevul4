static int start_command_port(struct usb_serial *serial)
{
	struct usb_serial_port *command_port;
	struct whiteheat_command_private *command_info;
	int retval = 0;

	command_port = serial->port[COMMAND_PORT];
	command_info = usb_get_serial_port_data(command_port);
	mutex_lock(&command_info->mutex);
	if (!command_info->port_running) {
		/* Work around HCD bugs */
		usb_clear_halt(serial->dev, command_port->read_urb->pipe);

		retval = usb_submit_urb(command_port->read_urb, GFP_KERNEL);
		if (retval) {
			dev_err(&serial->dev->dev,
				"%s - failed submitting read urb, error %d\n",
				__func__, retval);
			goto exit;
		}
	}
	command_info->port_running++;

exit:
	mutex_unlock(&command_info->mutex);
	return retval;
}
