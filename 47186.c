void oz_hcd_pd_reset(void *hpd, void *hport)
{
	/* Cleanup the current configuration and report reset to the core.
	 */
	struct oz_port *port = hport;
	struct oz_hcd *ozhcd = port->ozhcd;

	oz_dbg(ON, "PD Reset\n");
	spin_lock_bh(&port->port_lock);
	port->flags |= OZ_PORT_F_CHANGED;
	port->status |= USB_PORT_STAT_RESET;
	port->status |= (USB_PORT_STAT_C_RESET << 16);
	spin_unlock_bh(&port->port_lock);
	oz_clean_endpoints_for_config(ozhcd->hcd, port);
	usb_hcd_poll_rh_status(ozhcd->hcd);
}
