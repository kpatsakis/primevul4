void oz_hcd_pd_departed(struct oz_port *port)
{
	struct oz_hcd *ozhcd;
	void *hpd;
	struct oz_endpoint *ep = NULL;

	if (port == NULL) {
		oz_dbg(ON, "%s: port = 0\n", __func__);
		return;
	}
	ozhcd = port->ozhcd;
	if (ozhcd == NULL)
		return;
	/* Check if this is the connection port - if so clear it.
	 */
	spin_lock_bh(&ozhcd->hcd_lock);
	if ((ozhcd->conn_port >= 0) &&
		(port == &ozhcd->ports[ozhcd->conn_port])) {
		oz_dbg(ON, "Clearing conn_port\n");
		ozhcd->conn_port = -1;
	}
	spin_lock(&port->port_lock);
	port->flags |= OZ_PORT_F_DYING;
	spin_unlock(&port->port_lock);
	spin_unlock_bh(&ozhcd->hcd_lock);

	oz_clean_endpoints_for_config(ozhcd->hcd, port);
	spin_lock_bh(&port->port_lock);
	hpd = port->hpd;
	port->hpd = NULL;
	port->bus_addr = 0xff;
	port->config_num = 0;
	port->flags &= ~(OZ_PORT_F_PRESENT | OZ_PORT_F_DYING);
	port->flags |= OZ_PORT_F_CHANGED;
	port->status &= ~(USB_PORT_STAT_CONNECTION | USB_PORT_STAT_ENABLE);
	port->status |= (USB_PORT_STAT_C_CONNECTION << 16);
	/* If there is an endpont 0 then clear the pointer while we hold
	 * the spinlock be we deallocate it after releasing the lock.
	 */
	if (port->out_ep[0]) {
		ep = port->out_ep[0];
		port->out_ep[0] = NULL;
	}
	spin_unlock_bh(&port->port_lock);
	if (ep)
		oz_ep_free(port, ep);
	usb_hcd_poll_rh_status(ozhcd->hcd);
	oz_usb_put(hpd);
}
