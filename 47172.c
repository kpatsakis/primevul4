static int oz_hcd_bus_resume(struct usb_hcd *hcd)
{
	struct oz_hcd *ozhcd;

	ozhcd = oz_hcd_private(hcd);
	spin_lock_bh(&ozhcd->hcd_lock);
	ozhcd->flags &= ~OZ_HDC_F_SUSPENDED;
	hcd->state = HC_STATE_RUNNING;
	spin_unlock_bh(&ozhcd->hcd_lock);
	return 0;
}
