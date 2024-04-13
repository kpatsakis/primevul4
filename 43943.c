static int firm_send_command(struct usb_serial_port *port, __u8 command,
						__u8 *data, __u8 datasize)
{
	struct usb_serial_port *command_port;
	struct whiteheat_command_private *command_info;
	struct whiteheat_private *info;
	struct device *dev = &port->dev;
	__u8 *transfer_buffer;
	int retval = 0;
	int t;

	dev_dbg(dev, "%s - command %d\n", __func__, command);

	command_port = port->serial->port[COMMAND_PORT];
	command_info = usb_get_serial_port_data(command_port);
	mutex_lock(&command_info->mutex);
	command_info->command_finished = false;

	transfer_buffer = (__u8 *)command_port->write_urb->transfer_buffer;
	transfer_buffer[0] = command;
	memcpy(&transfer_buffer[1], data, datasize);
	command_port->write_urb->transfer_buffer_length = datasize + 1;
	retval = usb_submit_urb(command_port->write_urb, GFP_NOIO);
	if (retval) {
		dev_dbg(dev, "%s - submit urb failed\n", __func__);
		goto exit;
	}

	/* wait for the command to complete */
	t = wait_event_timeout(command_info->wait_command,
		(bool)command_info->command_finished, COMMAND_TIMEOUT);
	if (!t)
		usb_kill_urb(command_port->write_urb);

	if (command_info->command_finished == false) {
		dev_dbg(dev, "%s - command timed out.\n", __func__);
		retval = -ETIMEDOUT;
		goto exit;
	}

	if (command_info->command_finished == WHITEHEAT_CMD_FAILURE) {
		dev_dbg(dev, "%s - command failed.\n", __func__);
		retval = -EIO;
		goto exit;
	}

	if (command_info->command_finished == WHITEHEAT_CMD_COMPLETE) {
		dev_dbg(dev, "%s - command completed.\n", __func__);
		switch (command) {
		case WHITEHEAT_GET_DTR_RTS:
			info = usb_get_serial_port_data(port);
			memcpy(&info->mcr, command_info->result_buffer,
					sizeof(struct whiteheat_dr_info));
				break;
		}
	}
exit:
	mutex_unlock(&command_info->mutex);
	return retval;
}
