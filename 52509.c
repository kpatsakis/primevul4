static void cypress_send(struct usb_serial_port *port)
{
	int count = 0, result, offset, actual_size;
	struct cypress_private *priv = usb_get_serial_port_data(port);
	struct device *dev = &port->dev;
	unsigned long flags;

	if (!priv->comm_is_ok)
		return;

	dev_dbg(dev, "%s - interrupt out size is %d\n", __func__,
		port->interrupt_out_size);

	spin_lock_irqsave(&priv->lock, flags);
	if (priv->write_urb_in_use) {
		dev_dbg(dev, "%s - can't write, urb in use\n", __func__);
		spin_unlock_irqrestore(&priv->lock, flags);
		return;
	}
	spin_unlock_irqrestore(&priv->lock, flags);

	/* clear buffer */
	memset(port->interrupt_out_urb->transfer_buffer, 0,
						port->interrupt_out_size);

	spin_lock_irqsave(&priv->lock, flags);
	switch (priv->pkt_fmt) {
	default:
	case packet_format_1:
		/* this is for the CY7C64013... */
		offset = 2;
		port->interrupt_out_buffer[0] = priv->line_control;
		break;
	case packet_format_2:
		/* this is for the CY7C63743... */
		offset = 1;
		port->interrupt_out_buffer[0] = priv->line_control;
		break;
	}

	if (priv->line_control & CONTROL_RESET)
		priv->line_control &= ~CONTROL_RESET;

	if (priv->cmd_ctrl) {
		priv->cmd_count++;
		dev_dbg(dev, "%s - line control command being issued\n", __func__);
		spin_unlock_irqrestore(&priv->lock, flags);
		goto send;
	} else
		spin_unlock_irqrestore(&priv->lock, flags);

	count = kfifo_out_locked(&priv->write_fifo,
					&port->interrupt_out_buffer[offset],
					port->interrupt_out_size - offset,
					&priv->lock);
	if (count == 0)
		return;

	switch (priv->pkt_fmt) {
	default:
	case packet_format_1:
		port->interrupt_out_buffer[1] = count;
		break;
	case packet_format_2:
		port->interrupt_out_buffer[0] |= count;
	}

	dev_dbg(dev, "%s - count is %d\n", __func__, count);

send:
	spin_lock_irqsave(&priv->lock, flags);
	priv->write_urb_in_use = 1;
	spin_unlock_irqrestore(&priv->lock, flags);

	if (priv->cmd_ctrl)
		actual_size = 1;
	else
		actual_size = count +
			      (priv->pkt_fmt == packet_format_1 ? 2 : 1);

	usb_serial_debug_data(dev, __func__, port->interrupt_out_size,
			      port->interrupt_out_urb->transfer_buffer);

	usb_fill_int_urb(port->interrupt_out_urb, port->serial->dev,
		usb_sndintpipe(port->serial->dev, port->interrupt_out_endpointAddress),
		port->interrupt_out_buffer, port->interrupt_out_size,
		cypress_write_int_callback, port, priv->write_urb_interval);
	result = usb_submit_urb(port->interrupt_out_urb, GFP_ATOMIC);
	if (result) {
		dev_err_console(port,
				"%s - failed submitting write urb, error %d\n",
							__func__, result);
		priv->write_urb_in_use = 0;
		cypress_set_dead(port);
	}

	spin_lock_irqsave(&priv->lock, flags);
	if (priv->cmd_ctrl)
		priv->cmd_ctrl = 0;

	/* do not count the line control and size bytes */
	priv->bytes_out += count;
	spin_unlock_irqrestore(&priv->lock, flags);

	usb_serial_port_softint(port);
} /* cypress_send */
