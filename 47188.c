static inline void oz_hcd_put(struct oz_hcd *ozhcd)
{
	if (ozhcd)
		usb_put_hcd(ozhcd->hcd);
}
