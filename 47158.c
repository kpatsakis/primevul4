static void oz_clean_endpoints_for_config(struct usb_hcd *hcd,
			struct oz_port *port)
{
	struct oz_hcd *ozhcd = port->ozhcd;
	int i;

	oz_dbg(ON, "Deleting endpoints for configuration\n");
	for (i = 0; i < port->num_iface; i++)
		oz_clean_endpoints_for_interface(hcd, port, i);
	spin_lock_bh(&ozhcd->hcd_lock);
	if (port->iface) {
		oz_dbg(ON, "Freeing interfaces object\n");
		kfree(port->iface);
		port->iface = NULL;
	}
	port->num_iface = 0;
	spin_unlock_bh(&ozhcd->hcd_lock);
}
