static int oz_hcd_hub_status_data(struct usb_hcd *hcd, char *buf)
{
	struct oz_hcd *ozhcd = oz_hcd_private(hcd);
	int i;

	buf[0] = 0;
	buf[1] = 0;

	spin_lock_bh(&ozhcd->hcd_lock);
	for (i = 0; i < OZ_NB_PORTS; i++) {
		if (ozhcd->ports[i].flags & OZ_PORT_F_CHANGED) {
			oz_dbg(HUB, "Port %d changed\n", i);
			ozhcd->ports[i].flags &= ~OZ_PORT_F_CHANGED;
			if (i < 7)
				buf[0] |= 1 << (i + 1);
			else
				buf[1] |= 1 << (i - 7);
		}
	}
	spin_unlock_bh(&ozhcd->hcd_lock);
	if (buf[0] != 0 || buf[1] != 0)
		return 2;
	return 0;
}
