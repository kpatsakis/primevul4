static inline struct oz_hcd *oz_hcd_private(struct usb_hcd *hcd)
{
	return (struct oz_hcd *)hcd->hcd_priv;
}
