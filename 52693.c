static int palm_os_3_probe(struct usb_serial *serial,
						const struct usb_device_id *id)
{
	struct device *dev = &serial->dev->dev;
	struct visor_connection_info *connection_info;
	unsigned char *transfer_buffer;
	char *string;
	int retval = 0;
	int i;
	int num_ports = 0;

	transfer_buffer = kmalloc(sizeof(*connection_info), GFP_KERNEL);
	if (!transfer_buffer)
		return -ENOMEM;

	/* send a get connection info request */
	retval = usb_control_msg(serial->dev,
				  usb_rcvctrlpipe(serial->dev, 0),
				  VISOR_GET_CONNECTION_INFORMATION,
				  0xc2, 0x0000, 0x0000, transfer_buffer,
				  sizeof(*connection_info), 300);
	if (retval < 0) {
		dev_err(dev, "%s - error %d getting connection information\n",
			__func__, retval);
		goto exit;
	}

	if (retval == sizeof(*connection_info)) {
			connection_info = (struct visor_connection_info *)
							transfer_buffer;

		num_ports = le16_to_cpu(connection_info->num_ports);
		for (i = 0; i < num_ports; ++i) {
			switch (
			   connection_info->connections[i].port_function_id) {
			case VISOR_FUNCTION_GENERIC:
				string = "Generic";
				break;
			case VISOR_FUNCTION_DEBUGGER:
				string = "Debugger";
				break;
			case VISOR_FUNCTION_HOTSYNC:
				string = "HotSync";
				break;
			case VISOR_FUNCTION_CONSOLE:
				string = "Console";
				break;
			case VISOR_FUNCTION_REMOTE_FILE_SYS:
				string = "Remote File System";
				break;
			default:
				string = "unknown";
				break;
			}
			dev_info(dev, "%s: port %d, is for %s use\n",
				serial->type->description,
				connection_info->connections[i].port, string);
		}
	}
	/*
	* Handle devices that report invalid stuff here.
	*/
	if (num_ports == 0 || num_ports > 2) {
		dev_warn(dev, "%s: No valid connect info available\n",
			serial->type->description);
		num_ports = 2;
	}

	dev_info(dev, "%s: Number of ports: %d\n", serial->type->description,
		num_ports);

	/*
	 * save off our num_ports info so that we can use it in the
	 * calc_num_ports callback
	 */
	usb_set_serial_data(serial, (void *)(long)num_ports);

	/* ask for the number of bytes available, but ignore the
	   response as it is broken */
	retval = usb_control_msg(serial->dev,
				  usb_rcvctrlpipe(serial->dev, 0),
				  VISOR_REQUEST_BYTES_AVAILABLE,
				  0xc2, 0x0000, 0x0005, transfer_buffer,
				  0x02, 300);
	if (retval < 0)
		dev_err(dev, "%s - error %d getting bytes available request\n",
			__func__, retval);
	retval = 0;

exit:
	kfree(transfer_buffer);

	return retval;
}
