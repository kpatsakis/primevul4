static int digi_write_oob_command(struct usb_serial_port *port,
	unsigned char *buf, int count, int interruptible)
{
	int ret = 0;
	int len;
	struct usb_serial_port *oob_port = (struct usb_serial_port *)((struct digi_serial *)(usb_get_serial_data(port->serial)))->ds_oob_port;
	struct digi_port *oob_priv = usb_get_serial_port_data(oob_port);
	unsigned long flags = 0;

	dev_dbg(&port->dev,
		"digi_write_oob_command: TOP: port=%d, count=%d\n",
		oob_priv->dp_port_num, count);

	spin_lock_irqsave(&oob_priv->dp_port_lock, flags);
	while (count > 0) {
		while (oob_priv->dp_write_urb_in_use) {
			cond_wait_interruptible_timeout_irqrestore(
				&oob_port->write_wait, DIGI_RETRY_TIMEOUT,
				&oob_priv->dp_port_lock, flags);
			if (interruptible && signal_pending(current))
				return -EINTR;
			spin_lock_irqsave(&oob_priv->dp_port_lock, flags);
		}

		/* len must be a multiple of 4, so commands are not split */
		len = min(count, oob_port->bulk_out_size);
		if (len > 4)
			len &= ~3;
		memcpy(oob_port->write_urb->transfer_buffer, buf, len);
		oob_port->write_urb->transfer_buffer_length = len;
		ret = usb_submit_urb(oob_port->write_urb, GFP_ATOMIC);
		if (ret == 0) {
			oob_priv->dp_write_urb_in_use = 1;
			count -= len;
			buf += len;
		}
	}
	spin_unlock_irqrestore(&oob_priv->dp_port_lock, flags);
	if (ret)
		dev_err(&port->dev, "%s: usb_submit_urb failed, ret=%d\n",
			__func__, ret);
	return ret;

}
