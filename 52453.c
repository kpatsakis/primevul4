static int digi_write(struct tty_struct *tty, struct usb_serial_port *port,
					const unsigned char *buf, int count)
{

	int ret, data_len, new_len;
	struct digi_port *priv = usb_get_serial_port_data(port);
	unsigned char *data = port->write_urb->transfer_buffer;
	unsigned long flags = 0;

	dev_dbg(&port->dev,
		"digi_write: TOP: port=%d, count=%d, in_interrupt=%ld\n",
		priv->dp_port_num, count, in_interrupt());

	/* copy user data (which can sleep) before getting spin lock */
	count = min(count, port->bulk_out_size-2);
	count = min(64, count);

	/* be sure only one write proceeds at a time */
	/* there are races on the port private buffer */
	spin_lock_irqsave(&priv->dp_port_lock, flags);

	/* wait for urb status clear to submit another urb */
	if (priv->dp_write_urb_in_use) {
		/* buffer data if count is 1 (probably put_char) if possible */
		if (count == 1 && priv->dp_out_buf_len < DIGI_OUT_BUF_SIZE) {
			priv->dp_out_buf[priv->dp_out_buf_len++] = *buf;
			new_len = 1;
		} else {
			new_len = 0;
		}
		spin_unlock_irqrestore(&priv->dp_port_lock, flags);
		return new_len;
	}

	/* allow space for any buffered data and for new data, up to */
	/* transfer buffer size - 2 (for command and length bytes) */
	new_len = min(count, port->bulk_out_size-2-priv->dp_out_buf_len);
	data_len = new_len + priv->dp_out_buf_len;

	if (data_len == 0) {
		spin_unlock_irqrestore(&priv->dp_port_lock, flags);
		return 0;
	}

	port->write_urb->transfer_buffer_length = data_len+2;

	*data++ = DIGI_CMD_SEND_DATA;
	*data++ = data_len;

	/* copy in buffered data first */
	memcpy(data, priv->dp_out_buf, priv->dp_out_buf_len);
	data += priv->dp_out_buf_len;

	/* copy in new data */
	memcpy(data, buf, new_len);

	ret = usb_submit_urb(port->write_urb, GFP_ATOMIC);
	if (ret == 0) {
		priv->dp_write_urb_in_use = 1;
		ret = new_len;
		priv->dp_out_buf_len = 0;
	}

	/* return length of new data written, or error */
	spin_unlock_irqrestore(&priv->dp_port_lock, flags);
	if (ret < 0)
		dev_err_console(port,
			"%s: usb_submit_urb failed, ret=%d, port=%d\n",
			__func__, ret, priv->dp_port_num);
	dev_dbg(&port->dev, "digi_write: returning %d\n", ret);
	return ret;

}
