static void *oz_claim_hpd(struct oz_port *port)
{
	void *hpd;
	struct oz_hcd *ozhcd = port->ozhcd;

	spin_lock_bh(&ozhcd->hcd_lock);
	hpd = port->hpd;
	if (hpd)
		oz_usb_get(hpd);
	spin_unlock_bh(&ozhcd->hcd_lock);
	return hpd;
}
