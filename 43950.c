static int whiteheat_attach(struct usb_serial *serial)
{
	struct usb_serial_port *command_port;
	struct whiteheat_command_private *command_info;
	struct whiteheat_hw_info *hw_info;
	int pipe;
	int ret;
	int alen;
	__u8 *command;
	__u8 *result;

	command_port = serial->port[COMMAND_PORT];

	pipe = usb_sndbulkpipe(serial->dev,
			command_port->bulk_out_endpointAddress);
	command = kmalloc(2, GFP_KERNEL);
	if (!command)
		goto no_command_buffer;
	command[0] = WHITEHEAT_GET_HW_INFO;
	command[1] = 0;

	result = kmalloc(sizeof(*hw_info) + 1, GFP_KERNEL);
	if (!result)
		goto no_result_buffer;
	/*
	 * When the module is reloaded the firmware is still there and
	 * the endpoints are still in the usb core unchanged. This is the
	 * unlinking bug in disguise. Same for the call below.
	 */
	usb_clear_halt(serial->dev, pipe);
	ret = usb_bulk_msg(serial->dev, pipe, command, 2,
						&alen, COMMAND_TIMEOUT_MS);
	if (ret) {
		dev_err(&serial->dev->dev, "%s: Couldn't send command [%d]\n",
			serial->type->description, ret);
		goto no_firmware;
	} else if (alen != 2) {
		dev_err(&serial->dev->dev, "%s: Send command incomplete [%d]\n",
			serial->type->description, alen);
		goto no_firmware;
	}

	pipe = usb_rcvbulkpipe(serial->dev,
				command_port->bulk_in_endpointAddress);
	/* See the comment on the usb_clear_halt() above */
	usb_clear_halt(serial->dev, pipe);
	ret = usb_bulk_msg(serial->dev, pipe, result,
			sizeof(*hw_info) + 1, &alen, COMMAND_TIMEOUT_MS);
	if (ret) {
		dev_err(&serial->dev->dev, "%s: Couldn't get results [%d]\n",
			serial->type->description, ret);
		goto no_firmware;
	} else if (alen != sizeof(*hw_info) + 1) {
		dev_err(&serial->dev->dev, "%s: Get results incomplete [%d]\n",
			serial->type->description, alen);
		goto no_firmware;
	} else if (result[0] != command[0]) {
		dev_err(&serial->dev->dev, "%s: Command failed [%d]\n",
			serial->type->description, result[0]);
		goto no_firmware;
	}

	hw_info = (struct whiteheat_hw_info *)&result[1];

	dev_info(&serial->dev->dev, "%s: Firmware v%d.%02d\n",
		 serial->type->description,
		 hw_info->sw_major_rev, hw_info->sw_minor_rev);

	command_info = kmalloc(sizeof(struct whiteheat_command_private),
								GFP_KERNEL);
	if (!command_info)
		goto no_command_private;

	mutex_init(&command_info->mutex);
	command_info->port_running = 0;
	init_waitqueue_head(&command_info->wait_command);
	usb_set_serial_port_data(command_port, command_info);
	command_port->write_urb->complete = command_port_write_callback;
	command_port->read_urb->complete = command_port_read_callback;
	kfree(result);
	kfree(command);

	return 0;

no_firmware:
	/* Firmware likely not running */
	dev_err(&serial->dev->dev,
		"%s: Unable to retrieve firmware version, try replugging\n",
		serial->type->description);
	dev_err(&serial->dev->dev,
		"%s: If the firmware is not running (status led not blinking)\n",
		serial->type->description);
	dev_err(&serial->dev->dev,
		"%s: please contact support@connecttech.com\n",
		serial->type->description);
	kfree(result);
	kfree(command);
	return -ENODEV;

no_command_private:
	kfree(result);
no_result_buffer:
	kfree(command);
no_command_buffer:
	return -ENOMEM;
}
