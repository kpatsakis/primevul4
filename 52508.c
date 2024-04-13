static void cypress_read_int_callback(struct urb *urb)
{
	struct usb_serial_port *port = urb->context;
	struct cypress_private *priv = usb_get_serial_port_data(port);
	struct device *dev = &urb->dev->dev;
	struct tty_struct *tty;
	unsigned char *data = urb->transfer_buffer;
	unsigned long flags;
	char tty_flag = TTY_NORMAL;
	int havedata = 0;
	int bytes = 0;
	int result;
	int i = 0;
	int status = urb->status;

	switch (status) {
	case 0: /* success */
		break;
	case -ECONNRESET:
	case -ENOENT:
	case -ESHUTDOWN:
		/* precursor to disconnect so just go away */
		return;
	case -EPIPE:
		/* Can't call usb_clear_halt while in_interrupt */
		/* FALLS THROUGH */
	default:
		/* something ugly is going on... */
		dev_err(dev, "%s - unexpected nonzero read status received: %d\n",
			__func__, status);
		cypress_set_dead(port);
		return;
	}

	spin_lock_irqsave(&priv->lock, flags);
	if (priv->rx_flags & THROTTLED) {
		dev_dbg(dev, "%s - now throttling\n", __func__);
		priv->rx_flags |= ACTUALLY_THROTTLED;
		spin_unlock_irqrestore(&priv->lock, flags);
		return;
	}
	spin_unlock_irqrestore(&priv->lock, flags);

	tty = tty_port_tty_get(&port->port);
	if (!tty) {
		dev_dbg(dev, "%s - bad tty pointer - exiting\n", __func__);
		return;
	}

	spin_lock_irqsave(&priv->lock, flags);
	result = urb->actual_length;
	switch (priv->pkt_fmt) {
	default:
	case packet_format_1:
		/* This is for the CY7C64013... */
		priv->current_status = data[0] & 0xF8;
		bytes = data[1] + 2;
		i = 2;
		if (bytes > 2)
			havedata = 1;
		break;
	case packet_format_2:
		/* This is for the CY7C63743... */
		priv->current_status = data[0] & 0xF8;
		bytes = (data[0] & 0x07) + 1;
		i = 1;
		if (bytes > 1)
			havedata = 1;
		break;
	}
	spin_unlock_irqrestore(&priv->lock, flags);
	if (result < bytes) {
		dev_dbg(dev,
			"%s - wrong packet size - received %d bytes but packet said %d bytes\n",
			__func__, result, bytes);
		goto continue_read;
	}

	usb_serial_debug_data(&port->dev, __func__, urb->actual_length, data);

	spin_lock_irqsave(&priv->lock, flags);
	/* check to see if status has changed */
	if (priv->current_status != priv->prev_status) {
		u8 delta = priv->current_status ^ priv->prev_status;

		if (delta & UART_MSR_MASK) {
			if (delta & UART_CTS)
				port->icount.cts++;
			if (delta & UART_DSR)
				port->icount.dsr++;
			if (delta & UART_RI)
				port->icount.rng++;
			if (delta & UART_CD)
				port->icount.dcd++;

			wake_up_interruptible(&port->port.delta_msr_wait);
		}

		priv->prev_status = priv->current_status;
	}
	spin_unlock_irqrestore(&priv->lock, flags);

	/* hangup, as defined in acm.c... this might be a bad place for it
	 * though */
	if (tty && !C_CLOCAL(tty) && !(priv->current_status & UART_CD)) {
		dev_dbg(dev, "%s - calling hangup\n", __func__);
		tty_hangup(tty);
		goto continue_read;
	}

	/* There is one error bit... I'm assuming it is a parity error
	 * indicator as the generic firmware will set this bit to 1 if a
	 * parity error occurs.
	 * I can not find reference to any other error events. */
	spin_lock_irqsave(&priv->lock, flags);
	if (priv->current_status & CYP_ERROR) {
		spin_unlock_irqrestore(&priv->lock, flags);
		tty_flag = TTY_PARITY;
		dev_dbg(dev, "%s - Parity Error detected\n", __func__);
	} else
		spin_unlock_irqrestore(&priv->lock, flags);

	/* process read if there is data other than line status */
	if (bytes > i) {
		tty_insert_flip_string_fixed_flag(&port->port, data + i,
				tty_flag, bytes - i);
		tty_flip_buffer_push(&port->port);
	}

	spin_lock_irqsave(&priv->lock, flags);
	/* control and status byte(s) are also counted */
	priv->bytes_in += bytes;
	spin_unlock_irqrestore(&priv->lock, flags);

continue_read:
	tty_kref_put(tty);

	/* Continue trying to always read */

	if (priv->comm_is_ok) {
		usb_fill_int_urb(port->interrupt_in_urb, port->serial->dev,
				usb_rcvintpipe(port->serial->dev,
					port->interrupt_in_endpointAddress),
				port->interrupt_in_urb->transfer_buffer,
				port->interrupt_in_urb->transfer_buffer_length,
				cypress_read_int_callback, port,
				priv->read_urb_interval);
		result = usb_submit_urb(port->interrupt_in_urb, GFP_ATOMIC);
		if (result && result != -EPERM) {
			dev_err(dev, "%s - failed resubmitting read urb, error %d\n",
				__func__, result);
			cypress_set_dead(port);
		}
	}
} /* cypress_read_int_callback */
