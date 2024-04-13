static void cypress_close(struct usb_serial_port *port)
{
	struct cypress_private *priv = usb_get_serial_port_data(port);
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);
	kfifo_reset_out(&priv->write_fifo);
	spin_unlock_irqrestore(&priv->lock, flags);

	dev_dbg(&port->dev, "%s - stopping urbs\n", __func__);
	usb_kill_urb(port->interrupt_in_urb);
	usb_kill_urb(port->interrupt_out_urb);

	if (stats)
		dev_info(&port->dev, "Statistics: %d Bytes In | %d Bytes Out | %d Commands Issued\n",
			priv->bytes_in, priv->bytes_out, priv->cmd_count);
} /* cypress_close */
