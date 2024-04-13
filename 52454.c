static void digi_write_bulk_callback(struct urb *urb)
{

	struct usb_serial_port *port = urb->context;
	struct usb_serial *serial;
	struct digi_port *priv;
	struct digi_serial *serial_priv;
	int ret = 0;
	int status = urb->status;

	/* port and serial sanity check */
	if (port == NULL || (priv = usb_get_serial_port_data(port)) == NULL) {
		pr_err("%s: port or port->private is NULL, status=%d\n",
			__func__, status);
		return;
	}
	serial = port->serial;
	if (serial == NULL || (serial_priv = usb_get_serial_data(serial)) == NULL) {
		dev_err(&port->dev,
			"%s: serial or serial->private is NULL, status=%d\n",
			__func__, status);
		return;
	}

	/* handle oob callback */
	if (priv->dp_port_num == serial_priv->ds_oob_port_num) {
		dev_dbg(&port->dev, "digi_write_bulk_callback: oob callback\n");
		spin_lock(&priv->dp_port_lock);
		priv->dp_write_urb_in_use = 0;
		wake_up_interruptible(&port->write_wait);
		spin_unlock(&priv->dp_port_lock);
		return;
	}

	/* try to send any buffered data on this port */
	spin_lock(&priv->dp_port_lock);
	priv->dp_write_urb_in_use = 0;
	if (priv->dp_out_buf_len > 0) {
		*((unsigned char *)(port->write_urb->transfer_buffer))
			= (unsigned char)DIGI_CMD_SEND_DATA;
		*((unsigned char *)(port->write_urb->transfer_buffer) + 1)
			= (unsigned char)priv->dp_out_buf_len;
		port->write_urb->transfer_buffer_length =
						priv->dp_out_buf_len + 2;
		memcpy(port->write_urb->transfer_buffer + 2, priv->dp_out_buf,
			priv->dp_out_buf_len);
		ret = usb_submit_urb(port->write_urb, GFP_ATOMIC);
		if (ret == 0) {
			priv->dp_write_urb_in_use = 1;
			priv->dp_out_buf_len = 0;
		}
	}
	/* wake up processes sleeping on writes immediately */
	tty_port_tty_wakeup(&port->port);
	/* also queue up a wakeup at scheduler time, in case we */
	/* lost the race in write_chan(). */
	schedule_work(&priv->dp_wakeup_work);

	spin_unlock(&priv->dp_port_lock);
	if (ret && ret != -EPERM)
		dev_err_console(port,
			"%s: usb_submit_urb failed, ret=%d, port=%d\n",
			__func__, ret, priv->dp_port_num);
}
