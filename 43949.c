static void stop_command_port(struct usb_serial *serial)
{
	struct usb_serial_port *command_port;
	struct whiteheat_command_private *command_info;

	command_port = serial->port[COMMAND_PORT];
	command_info = usb_get_serial_port_data(command_port);
	mutex_lock(&command_info->mutex);
	command_info->port_running--;
	if (!command_info->port_running)
		usb_kill_urb(command_port->read_urb);
	mutex_unlock(&command_info->mutex);
}
