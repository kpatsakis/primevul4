struct oz_port *oz_hcd_pd_arrived(void *hpd)
{
	int i;
	struct oz_port *hport;
	struct oz_hcd *ozhcd;
	struct oz_endpoint *ep;

	ozhcd = oz_hcd_claim();
	if (!ozhcd)
		return NULL;
	/* Allocate an endpoint object in advance (before holding hcd lock) to
	 * use for out endpoint 0.
	 */
	ep = oz_ep_alloc(0, GFP_ATOMIC);
	if (!ep)
		goto err_put;

	spin_lock_bh(&ozhcd->hcd_lock);
	if (ozhcd->conn_port >= 0)
		goto err_unlock;

	for (i = 0; i < OZ_NB_PORTS; i++) {
		struct oz_port *port = &ozhcd->ports[i];

		spin_lock(&port->port_lock);
		if (!(port->flags & (OZ_PORT_F_PRESENT | OZ_PORT_F_CHANGED))) {
			oz_acquire_port(port, hpd);
			spin_unlock(&port->port_lock);
			break;
		}
		spin_unlock(&port->port_lock);
	}
	if (i == OZ_NB_PORTS)
		goto err_unlock;

	ozhcd->conn_port = i;
	hport = &ozhcd->ports[i];
	hport->out_ep[0] = ep;
	spin_unlock_bh(&ozhcd->hcd_lock);
	if (ozhcd->flags & OZ_HDC_F_SUSPENDED)
		usb_hcd_resume_root_hub(ozhcd->hcd);
	usb_hcd_poll_rh_status(ozhcd->hcd);
	oz_hcd_put(ozhcd);

	return hport;

err_unlock:
	spin_unlock_bh(&ozhcd->hcd_lock);
	oz_ep_free(NULL, ep);
err_put:
	oz_hcd_put(ozhcd);
	return NULL;
}
