static int klsi_105_port_probe(struct usb_serial_port *port)
{
	struct klsi_105_private *priv;

	priv = kmalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	/* set initial values for control structures */
	priv->cfg.pktlen    = 5;
	priv->cfg.baudrate  = kl5kusb105a_sio_b9600;
	priv->cfg.databits  = kl5kusb105a_dtb_8;
	priv->cfg.unknown1  = 0;
	priv->cfg.unknown2  = 1;

	priv->line_state    = 0;

	spin_lock_init(&priv->lock);

	/* priv->termios is left uninitialized until port opening */

	usb_set_serial_port_data(port, priv);

	return 0;
}
