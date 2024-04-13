static int oz_plat_remove(struct platform_device *dev)
{
	struct usb_hcd *hcd = platform_get_drvdata(dev);
	struct oz_hcd *ozhcd;

	if (hcd == NULL)
		return -1;
	ozhcd = oz_hcd_private(hcd);
	spin_lock_bh(&g_hcdlock);
	if (ozhcd == g_ozhcd)
		g_ozhcd = NULL;
	spin_unlock_bh(&g_hcdlock);
	oz_dbg(ON, "Clearing orphanage\n");
	oz_hcd_clear_orphanage(ozhcd, -EPIPE);
	oz_dbg(ON, "Removing hcd\n");
	usb_remove_hcd(hcd);
	usb_put_hcd(hcd);
	return 0;
}
