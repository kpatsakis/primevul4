static int digi_port_init(struct usb_serial_port *port, unsigned port_num)
{
	struct digi_port *priv;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	spin_lock_init(&priv->dp_port_lock);
	priv->dp_port_num = port_num;
	init_waitqueue_head(&priv->dp_transmit_idle_wait);
	init_waitqueue_head(&priv->dp_flush_wait);
	init_waitqueue_head(&priv->dp_close_wait);
	INIT_WORK(&priv->dp_wakeup_work, digi_wakeup_write_lock);
	priv->dp_port = port;

	init_waitqueue_head(&port->write_wait);

	usb_set_serial_port_data(port, priv);

	return 0;
}
