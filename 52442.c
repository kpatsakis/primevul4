static void digi_read_bulk_callback(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	struct digi_port *priv;
	struct digi_serial *serial_priv;
	int ret;
	int status = urb->status;

	/* port sanity check, do not resubmit if port is not valid */
	if (port == NULL)
		return;
	priv = usb_get_serial_port_data(port);
	if (priv == NULL) {
		dev_err(&port->dev, "%s: port->private is NULL, status=%d\n",
			__func__, status);
		return;
	}
	if (port->serial == NULL ||
		(serial_priv = usb_get_serial_data(port->serial)) == NULL) {
		dev_err(&port->dev, "%s: serial is bad or serial->private "
			"is NULL, status=%d\n", __func__, status);
		return;
	}

	/* do not resubmit urb if it has any status error */
	if (status) {
		dev_err(&port->dev,
			"%s: nonzero read bulk status: status=%d, port=%d\n",
			__func__, status, priv->dp_port_num);
		return;
	}

	/* handle oob or inb callback, do not resubmit if error */
	if (priv->dp_port_num == serial_priv->ds_oob_port_num) {
		if (digi_read_oob_callback(urb) != 0)
			return;
	} else {
		if (digi_read_inb_callback(urb) != 0)
			return;
	}

	/* continue read */
	ret = usb_submit_urb(urb, GFP_ATOMIC);
	if (ret != 0 && ret != -EPERM) {
		dev_err(&port->dev,
			"%s: failed resubmitting urb, ret=%d, port=%d\n",
			__func__, ret, priv->dp_port_num);
	}

}
