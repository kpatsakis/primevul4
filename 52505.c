static void cypress_dtr_rts(struct usb_serial_port *port, int on)
{
	struct cypress_private *priv = usb_get_serial_port_data(port);
	/* drop dtr and rts */
	spin_lock_irq(&priv->lock);
	if (on == 0)
		priv->line_control = 0;
	else 
		priv->line_control = CONTROL_DTR | CONTROL_RTS;
	priv->cmd_ctrl = 1;
	spin_unlock_irq(&priv->lock);
	cypress_write(NULL, port, NULL, 0);
}
