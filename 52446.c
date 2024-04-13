static int digi_set_modem_signals(struct usb_serial_port *port,
	unsigned int modem_signals, int interruptible)
{

	int ret;
	struct digi_port *port_priv = usb_get_serial_port_data(port);
	struct usb_serial_port *oob_port = (struct usb_serial_port *) ((struct digi_serial *)(usb_get_serial_data(port->serial)))->ds_oob_port;
	struct digi_port *oob_priv = usb_get_serial_port_data(oob_port);
	unsigned char *data = oob_port->write_urb->transfer_buffer;
	unsigned long flags = 0;


	dev_dbg(&port->dev,
		"digi_set_modem_signals: TOP: port=%d, modem_signals=0x%x\n",
		port_priv->dp_port_num, modem_signals);

	spin_lock_irqsave(&oob_priv->dp_port_lock, flags);
	spin_lock(&port_priv->dp_port_lock);

	while (oob_priv->dp_write_urb_in_use) {
		spin_unlock(&port_priv->dp_port_lock);
		cond_wait_interruptible_timeout_irqrestore(
			&oob_port->write_wait, DIGI_RETRY_TIMEOUT,
			&oob_priv->dp_port_lock, flags);
		if (interruptible && signal_pending(current))
			return -EINTR;
		spin_lock_irqsave(&oob_priv->dp_port_lock, flags);
		spin_lock(&port_priv->dp_port_lock);
	}
	data[0] = DIGI_CMD_SET_DTR_SIGNAL;
	data[1] = port_priv->dp_port_num;
	data[2] = (modem_signals & TIOCM_DTR) ?
					DIGI_DTR_ACTIVE : DIGI_DTR_INACTIVE;
	data[3] = 0;
	data[4] = DIGI_CMD_SET_RTS_SIGNAL;
	data[5] = port_priv->dp_port_num;
	data[6] = (modem_signals & TIOCM_RTS) ?
					DIGI_RTS_ACTIVE : DIGI_RTS_INACTIVE;
	data[7] = 0;

	oob_port->write_urb->transfer_buffer_length = 8;

	ret = usb_submit_urb(oob_port->write_urb, GFP_ATOMIC);
	if (ret == 0) {
		oob_priv->dp_write_urb_in_use = 1;
		port_priv->dp_modem_signals =
			(port_priv->dp_modem_signals&~(TIOCM_DTR|TIOCM_RTS))
			| (modem_signals&(TIOCM_DTR|TIOCM_RTS));
	}
	spin_unlock(&port_priv->dp_port_lock);
	spin_unlock_irqrestore(&oob_priv->dp_port_lock, flags);
	if (ret)
		dev_err(&port->dev, "%s: usb_submit_urb failed, ret=%d\n",
			__func__, ret);
	return ret;
}
