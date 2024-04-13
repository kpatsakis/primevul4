static int digi_startup_device(struct usb_serial *serial)
{
	int i, ret = 0;
	struct digi_serial *serial_priv = usb_get_serial_data(serial);
	struct usb_serial_port *port;

	/* be sure this happens exactly once */
	spin_lock(&serial_priv->ds_serial_lock);
	if (serial_priv->ds_device_started) {
		spin_unlock(&serial_priv->ds_serial_lock);
		return 0;
	}
	serial_priv->ds_device_started = 1;
	spin_unlock(&serial_priv->ds_serial_lock);

	/* start reading from each bulk in endpoint for the device */
	/* set USB_DISABLE_SPD flag for write bulk urbs */
	for (i = 0; i < serial->type->num_ports + 1; i++) {
		port = serial->port[i];
		ret = usb_submit_urb(port->read_urb, GFP_KERNEL);
		if (ret != 0) {
			dev_err(&port->dev,
				"%s: usb_submit_urb failed, ret=%d, port=%d\n",
				__func__, ret, i);
			break;
		}
	}
	return ret;
}
